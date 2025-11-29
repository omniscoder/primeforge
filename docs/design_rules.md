# Design rules (v0.1)

- PAM: default NGG (SpCas9 H840A). Configurable via `DesignConfig.pam_motifs`.
- Cut site: 3 bp upstream of PAM relative to the spacer (equivalently spacer_start + 17; for a forward NGG this is PAM_start - 3).
- PBS: enumerated length range (default 8–17), reverse complement of sequence upstream of the nick.
- RTT: enumerated length range (default 10–40), must cover the edited bases plus buffer.
- Flags:
  - `flag_edit_far` if edit is farther than `max_nick_to_edit_distance` from the nick.
  - `flag_pbs_gc_extreme` if PBS GC < 0.30 or > 0.75.
- Ranking: deterministic order by cut index then spacer; scoring hooks come later (rule-based or ML).
- PE3 companion nicking sgRNA (optional): choose the nearest valid opposite-strand PAM; if none exist, fall back to the nearest alternate PAM on the same strand within `max_nick_to_edit_distance`.
- E2E tests: see `primeforge-core/tests/test_e2e.cpp` for plus/minus strand coverage and PE3 companion validation.

Sources informing these defaults: Addgene pegRNA design notes (PBS 8–17 with moderate GC) and PrimeDesign heuristics for keeping edits close to the nick.
