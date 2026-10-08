# Pokémon Emerald Throne: histórico de alterações

Linha do tempo das mudanças feitas sobre o `pokeemerald-expansion` (RHH 1.17.x).
Serve de base para a documentação oficial no futuro.

Comentários adicionados no código levam a marca `[Throne]` e são escritos em PT-BR,
para separar o que é nosso do que veio do expansion. Para achar todos:

```bash
grep -rn "\[Throne\]" include src data
```

Entradas mais novas ficam no topo.

---

## 2026-10-07

### Exp. Share entregue por NPC em Oldale

O Exp. Share deixa de vir no jogo novo. Agora uma moradora de Oldale entrega o item,
com fala própria, logo no começo da jornada.

- NPC: Pokéfan da esquerda na `OldaleTown_House2`, a casa residencial à direita da
  entrada de Oldale (sudeste do mapa).
- `data/maps/OldaleTown_House2/scripts.inc`: o script da moradora entrega
  `ITEM_EXP_SHARE`, liga `I_EXP_SHARE_FLAG` e explica o item. Depois disso ela só lembra
  como o item funciona. Três falas novas (`Text_TakeThisExpShare`, `Text_ExplainExpShare`,
  `Text_ExpShareReminder`) no lugar de `OldaleTown_House2_Text_PokemonLevelUp`.
- `include/constants/flags.h`: `FLAG_UNUSED_0x021` vira `FLAG_RECEIVED_EXP_SHARE_OLDALE`
  (`0x21`). Marca que o jogador já recebeu o item.
- `src/new_game.c`: removida a entrega do Exp. Share no `NewGameInitData`.

### Troca de prêmios: Sr. Stone e Mãe

Como o Exp. Share já vem desde o começo, os prêmios mudaram:

| Evento | Antes | Agora |
|---|---|---|
| Sr. Stone, Devon Corp 3F (entrega da carta) | Exp. Share | Amulet Coin |
| Mãe, em casa (depois da insígnia do Norman) | Amulet Coin | Lucky Egg |

- `data/maps/RustboroCity_DevonCorp_3F/scripts.inc`: entrega `ITEM_AMULET_COIN`. Novo
  texto `RustboroCity_DevonCorp_3F_Text_ExplainAmuletCoin` no lugar da explicação do
  Exp. Share.
- `data/scripts/players_house.inc`: entrega `ITEM_LUCKY_EGG`. Falas da Mãe não mudaram.
- Os nomes das flags (`FLAG_RECEIVED_EXP_SHARE`, `FLAG_RECEIVED_AMULET_COIN`) e dos
  labels dos scripts ficaram iguais, para não quebrar saves.

### Exp. Share ligado desde o começo (item chave)

Toda a equipe ganha XP desde o início do jogo. O Exp. Share funciona como item chave
estilo Gen 6, que liga e desliga o efeito.

- `include/config/item.h`
  - `I_EXP_SHARE_FLAG` = `FLAG_UNUSED_0x020` (antes `0`, que deixava o recurso desligado).
  - `I_EXP_SHARE_ITEM` = `GEN_6` (antes `GEN_5`).
- `src/new_game.c`: `NewGameInitData` põe o Exp. Share na mochila e liga a flag.
  *Substituído depois pela entrega em Oldale (ver acima).*

Observações:
- Só vale para jogo novo. Saves antigos não recebem o item nem a flag.
- A `FLAG_UNUSED_0x020` agora está em uso. Não reaproveitar.

### Relógio do jogo por frames (Fake RTC)

O relógio do jogo deixa de usar o RTC do cartucho e passa a contar frames emulados.
Assim acelera junto com o fast forward do emulador.

- `include/config/overworld.h`: `OW_USE_FAKE_RTC` = `TRUE` (antes `FALSE`).
- `src/fake_rtc.c` / `include/fake_rtc.h`: nova `FakeRtc_UpdateFrame`, com contador de
  frames próprio. Um segundo do relógio passa a cada 60 frames.
- `src/play_time.c`: `PlayTimeCounter_Update` chama `FakeRtc_UpdateFrame` em todo frame.
  Antes o relógio só andava junto com o tempo de jogo e parava quando este travava em
  999:59:59.

Commits: `8671330527`, `c4398d8a35`.

### Repositório

- `.gitignore` ignora arquivos de configuração de assistentes de IA (`.claude/`,
  `.cursor/`, `CLAUDE.md`, `AGENTS.md` etc.). Commit `340357cbc1`.
