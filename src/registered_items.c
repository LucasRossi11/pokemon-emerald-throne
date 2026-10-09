#include "global.h"
#include "registered_items.h"
#include "event_object_lock.h"
#include "item.h"
#include "item_icon.h"
#include "item_menu.h"
#include "list_menu.h"
#include "main.h"
#include "menu.h"
#include "script.h"
#include "script_menu.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "window.h"
#include "constants/songs.h"

// [Throne] Até MAX_REGISTERED_ITEMS Key Items registrados no SELECT ao mesmo tempo.
// Os espaços ficam em gSaveBlock1Ptr->registeredItems, na ordem em que foram registrados,
// sempre juntos no começo da lista (ITEM_NONE só no fim).

#define TAG_REGISTERED_ITEM_ICON 3001

// Menu rápido no canto superior direito, com o ícone do item selecionado à esquerda.
#define MENU_TOP         0
#define ICON_WINDOW_SIZE 4 // Em tiles

#define tListTaskId data[0]
#define tWindowId   data[1]

static EWRAM_DATA struct ListMenuItem sMenuItems[MAX_REGISTERED_ITEMS] = {0};
static EWRAM_DATA u8 sIconWindowId = 0;
static EWRAM_DATA u8 sIconSpriteId = 0;
static EWRAM_DATA u16 sLastUsedItem = ITEM_NONE; // O cursor abre no último item usado pelo menu.

static void Task_RegisteredItemsMenu(u8 taskId);
static void MoveCursor_ShowItemIcon(s32 itemId, bool8 onInit, struct ListMenu *list);

static const struct ListMenuTemplate sRegisteredItemsListTemplate =
{
    .moveCursorFunc = MoveCursor_ShowItemIcon,
    .item_X = 8,
    .upText_Y = 1,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 1,
    .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
    .fontId = FONT_NORMAL,
};

void ClearRegisteredItems(void)
{
    u32 i;

    for (i = 0; i < MAX_REGISTERED_ITEMS; i++)
        gSaveBlock1Ptr->registeredItems[i] = ITEM_NONE;
}

bool32 IsItemRegistered(enum Item itemId)
{
    u32 i;

    if (itemId == ITEM_NONE)
        return FALSE;

    for (i = 0; i < MAX_REGISTERED_ITEMS; i++)
    {
        if (gSaveBlock1Ptr->registeredItems[i] == itemId)
            return TRUE;
    }
    return FALSE;
}

static void CompactRegisteredItems(void)
{
    u32 i, count = 0;

    for (i = 0; i < MAX_REGISTERED_ITEMS; i++)
    {
        if (gSaveBlock1Ptr->registeredItems[i] != ITEM_NONE)
            gSaveBlock1Ptr->registeredItems[count++] = gSaveBlock1Ptr->registeredItems[i];
    }
    for (; count < MAX_REGISTERED_ITEMS; count++)
        gSaveBlock1Ptr->registeredItems[count] = ITEM_NONE;
}

// Tira da lista os itens que saíram da bolsa e devolve quantos ficaram.
u32 RemoveMissingRegisteredItems(void)
{
    u32 i, count = 0;

    for (i = 0; i < MAX_REGISTERED_ITEMS; i++)
    {
        if (gSaveBlock1Ptr->registeredItems[i] == ITEM_NONE)
            continue;

        if (CheckBagHasItem(gSaveBlock1Ptr->registeredItems[i], 1))
            count++;
        else
            gSaveBlock1Ptr->registeredItems[i] = ITEM_NONE;
    }
    CompactRegisteredItems();
    return count;
}

// Registra no primeiro espaço livre. Devolve FALSE se todos estiverem ocupados.
bool32 RegisterItem(enum Item itemId)
{
    u32 count;

    if (itemId == ITEM_NONE)
        return FALSE;
    if (IsItemRegistered(itemId))
        return TRUE;

    count = RemoveMissingRegisteredItems();
    if (count >= MAX_REGISTERED_ITEMS)
        return FALSE;

    gSaveBlock1Ptr->registeredItems[count] = itemId;
    return TRUE;
}

void UnregisterItem(enum Item itemId)
{
    u32 i;

    for (i = 0; i < MAX_REGISTERED_ITEMS; i++)
    {
        if (gSaveBlock1Ptr->registeredItems[i] == itemId)
            gSaveBlock1Ptr->registeredItems[i] = ITEM_NONE;
    }
    CompactRegisteredItems();
}

static void DestroyItemIcon(void)
{
    if (sIconSpriteId == MAX_SPRITES)
        return;

    FreeSpriteTilesByTag(TAG_REGISTERED_ITEM_ICON);
    FreeSpritePaletteByTag(TAG_REGISTERED_ITEM_ICON);
    DestroySprite(&gSprites[sIconSpriteId]);
    sIconSpriteId = MAX_SPRITES;
}

static void MoveCursor_ShowItemIcon(s32 itemId, bool8 onInit, struct ListMenu *list)
{
    struct WindowTemplate *iconWindow = &gWindows[sIconWindowId].window;

    if (!onInit)
        PlaySE(SE_SELECT);

    DestroyItemIcon();
    sIconSpriteId = AddItemIconSprite(TAG_REGISTERED_ITEM_ICON, TAG_REGISTERED_ITEM_ICON, itemId);
    if (sIconSpriteId != MAX_SPRITES)
    {
        // O ícone tem 24x24 px no canto de um sprite 32x32, por isso o +20 em vez de +16.
        gSprites[sIconSpriteId].oam.priority = 0;
        gSprites[sIconSpriteId].x = iconWindow->tilemapLeft * TILE_WIDTH + 20;
        gSprites[sIconSpriteId].y = iconWindow->tilemapTop * TILE_HEIGHT + 20;
    }
}

// Chamado pelo SELECT com 2 ou mais itens registrados. Os controles do jogador já estão
// travados e os objetos congelados (UseRegisteredKeyItemOnField).
void OpenRegisteredItemsMenu(void)
{
    struct ListMenuTemplate template = sRegisteredItemsListTemplate;
    struct WindowTemplate iconTemplate;
    u32 i, count, width = 0, tileWidth, initialRow = 0;
    u8 taskId, windowId, left;

    count = RemoveMissingRegisteredItems();
    for (i = 0; i < count; i++)
    {
        enum Item itemId = gSaveBlock1Ptr->registeredItems[i];

        sMenuItems[i].name = GetItemName(itemId);
        sMenuItems[i].id = itemId;
        if (itemId == sLastUsedItem)
            initialRow = i;
        width = DisplayTextAndGetWidth(sMenuItems[i].name, width);
    }

    LoadMessageBoxAndBorderGfx();

    // CreateWindowFromRect soma 1 em x e y para caber a moldura.
    tileWidth = ConvertPixelWidthToTileWidth(width);
    left = DISPLAY_TILE_WIDTH - 2 - tileWidth;
    windowId = CreateWindowFromRect(left, MENU_TOP, tileWidth, count * 2);
    SetStandardWindowBorderStyle(windowId, FALSE);

    // Quadro do ícone, com um tile de folga até a moldura da lista. Usa os tiles logo
    // depois dos da lista (CreateWindowFromRect começa no tile 100).
    iconTemplate = CreateWindowTemplate(0, left - ICON_WINDOW_SIZE - 2, MENU_TOP + 1, ICON_WINDOW_SIZE, ICON_WINDOW_SIZE, 15, 100 + tileWidth * count * 2);
    sIconWindowId = AddWindow(&iconTemplate);
    SetStandardWindowBorderStyle(sIconWindowId, FALSE);
    FillWindowPixelBuffer(sIconWindowId, PIXEL_FILL(1));
    CopyWindowToVram(sIconWindowId, COPYWIN_FULL);
    sIconSpriteId = MAX_SPRITES;

    template.windowId = windowId;
    template.items = sMenuItems;
    template.totalItems = count;
    template.maxShowed = count;

    taskId = CreateTask(Task_RegisteredItemsMenu, 80);
    gTasks[taskId].tWindowId = windowId;
    gTasks[taskId].tListTaskId = ListMenuInit(&template, 0, initialRow);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void CloseRegisteredItemsMenu(u8 taskId)
{
    DestroyItemIcon();
    DestroyListMenuTask(gTasks[taskId].tListTaskId, NULL, NULL);
    ClearToTransparentAndRemoveWindow(gTasks[taskId].tWindowId);
    ClearToTransparentAndRemoveWindow(sIconWindowId);
    DestroyTask(taskId);
}

static void Task_RegisteredItemsMenu(u8 taskId)
{
    s32 input;

    // SELECT de novo fecha o menu, como o B.
    if (JOY_NEW(SELECT_BUTTON))
        input = LIST_CANCEL;
    else
        input = ListMenu_ProcessInput(gTasks[taskId].tListTaskId);

    switch (input)
    {
    case LIST_NOTHING_CHOSEN:
    case LIST_HEADER:
        break;
    case LIST_CANCEL:
        PlaySE(SE_SELECT);
        CloseRegisteredItemsMenu(taskId);
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        break;
    default:
        PlaySE(SE_SELECT);
        CloseRegisteredItemsMenu(taskId);
        sLastUsedItem = input;
        UseRegisteredKeyItem(input);
        break;
    }
}
