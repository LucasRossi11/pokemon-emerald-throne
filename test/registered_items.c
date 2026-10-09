#include "global.h"
#include "item.h"
#include "registered_items.h"
#include "test/test.h"

// [Throne] Até MAX_REGISTERED_ITEMS Key Items registrados no SELECT.

static const enum Item sKeyItems[] =
{
    ITEM_MACH_BIKE,
    ITEM_OLD_ROD,
    ITEM_GOOD_ROD,
    ITEM_SUPER_ROD,
    ITEM_DOWSING_MACHINE,
};

STATIC_ASSERT(ARRAY_COUNT(sKeyItems) > MAX_REGISTERED_ITEMS, NotEnoughKeyItemsForRegisteredItemsTest);

static void SetUpBagWithKeyItems(void)
{
    u32 i;

    ClearBag();
    ClearRegisteredItems();
    for (i = 0; i < ARRAY_COUNT(sKeyItems); i++)
        AddBagItem(sKeyItems[i], 1);
}

TEST("Registered items: up to MAX_REGISTERED_ITEMS can be registered at once")
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sKeyItems); i++)
        ASSUME(GetItemPocket(sKeyItems[i]) == POCKET_KEY_ITEMS);

    SetUpBagWithKeyItems();
    for (i = 0; i < MAX_REGISTERED_ITEMS; i++)
        EXPECT(RegisterItem(sKeyItems[i]));
    EXPECT(!RegisterItem(sKeyItems[MAX_REGISTERED_ITEMS]));

    for (i = 0; i < MAX_REGISTERED_ITEMS; i++)
        EXPECT_EQ(gSaveBlock1Ptr->registeredItems[i], sKeyItems[i]);
    EXPECT(!IsItemRegistered(sKeyItems[MAX_REGISTERED_ITEMS]));
}

TEST("Registered items: registering the same item twice uses one slot")
{
    SetUpBagWithKeyItems();
    EXPECT(RegisterItem(ITEM_OLD_ROD));
    EXPECT(RegisterItem(ITEM_OLD_ROD));
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[0], ITEM_OLD_ROD);
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[1], ITEM_NONE);
}

TEST("Registered items: unregistering keeps the remaining items in order")
{
    SetUpBagWithKeyItems();
    RegisterItem(ITEM_MACH_BIKE);
    RegisterItem(ITEM_OLD_ROD);
    RegisterItem(ITEM_GOOD_ROD);

    UnregisterItem(ITEM_OLD_ROD);

    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[0], ITEM_MACH_BIKE);
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[1], ITEM_GOOD_ROD);
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[2], ITEM_NONE);
    EXPECT(!IsItemRegistered(ITEM_OLD_ROD));
}

TEST("Registered items: items that left the bag free their slot")
{
    u32 i;

    SetUpBagWithKeyItems();
    for (i = 0; i < MAX_REGISTERED_ITEMS; i++)
        RegisterItem(sKeyItems[i]);

    RemoveBagItem(sKeyItems[0], 1);

    EXPECT_EQ(RemoveMissingRegisteredItems(), MAX_REGISTERED_ITEMS - 1);
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[0], sKeyItems[1]);
    EXPECT(RegisterItem(sKeyItems[MAX_REGISTERED_ITEMS]));
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[MAX_REGISTERED_ITEMS - 1], sKeyItems[MAX_REGISTERED_ITEMS]);
}

TEST("Registered items: SwapRegisteredBike swaps the bike in any slot")
{
    SetUpBagWithKeyItems();
    AddBagItem(ITEM_ACRO_BIKE, 1);
    RegisterItem(ITEM_OLD_ROD);
    RegisterItem(ITEM_MACH_BIKE);

    SwapRegisteredBike();

    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[0], ITEM_OLD_ROD);
    EXPECT_EQ(gSaveBlock1Ptr->registeredItems[1], ITEM_ACRO_BIKE);
}
