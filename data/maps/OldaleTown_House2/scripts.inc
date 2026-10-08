OldaleTown_House2_MapScripts::
	.byte 0

@ [Throne] Moradora entrega o Exp. Share (item chave Gen 6) e já deixa ele ligado.
OldaleTown_House2_EventScript_Woman::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_EXP_SHARE_OLDALE, OldaleTown_House2_EventScript_WomanAfterExpShare
	msgbox OldaleTown_House2_Text_TakeThisExpShare, MSGBOX_DEFAULT
	giveitem ITEM_EXP_SHARE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_EXP_SHARE_OLDALE
	setflag I_EXP_SHARE_FLAG
	msgbox OldaleTown_House2_Text_ExplainExpShare, MSGBOX_DEFAULT
	release
	end

OldaleTown_House2_EventScript_WomanAfterExpShare::
	msgbox OldaleTown_House2_Text_ExpShareReminder, MSGBOX_DEFAULT
	release
	end

OldaleTown_House2_EventScript_Man::
	msgbox OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon, MSGBOX_NPC
	end

@ [Throne] Falas novas da moradora, no lugar de OldaleTown_House2_Text_PokemonLevelUp.
OldaleTown_House2_Text_TakeThisExpShare:
	.string "Oh, you just got your first POKéMON?\p"
	.string "Raising POKéMON takes a lot of\n"
	.string "battles. I know that better than\l"
	.string "anyone.\p"
	.string "Here, take this. My kids don't need\n"
	.string "it anymore.$"

OldaleTown_House2_Text_ExplainExpShare:
	.string "That's an EXP. SHARE.\p"
	.string "While it's on, every POKéMON in your\n"
	.string "party gets EXP. Points, even the ones\l"
	.string "that didn't battle.\p"
	.string "You can turn it on or off from your\n"
	.string "KEY ITEMS pocket whenever you like.$"

OldaleTown_House2_Text_ExpShareReminder:
	.string "With the EXP. SHARE on, even the\n"
	.string "POKéMON waiting in your party will\l"
	.string "level up and get stronger.$"

OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon:
	.string "If the POKéMON with you become\n"
	.string "stronger, you'll be able to go farther\l"
	.string "away from here.$"

