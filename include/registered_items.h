#ifndef GUARD_REGISTERED_ITEMS_H
#define GUARD_REGISTERED_ITEMS_H

// [Throne] Até MAX_REGISTERED_ITEMS Key Items registrados no SELECT ao mesmo tempo.
// Com um registrado, o SELECT usa direto. Com mais, abre um menu rápido para escolher.

void ClearRegisteredItems(void);
bool32 IsItemRegistered(enum Item itemId);
bool32 RegisterItem(enum Item itemId);
void UnregisterItem(enum Item itemId);
u32 RemoveMissingRegisteredItems(void);
void OpenRegisteredItemsMenu(void);

#endif // GUARD_REGISTERED_ITEMS_H
