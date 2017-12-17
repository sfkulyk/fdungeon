// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

// wrapper function for safe command execution
void do_function args((CHAR_DATA *ch, DO_FUN *do_fun, const char *argument));
int get_vacant_deity();

// Structure for a command in the command lookup table.
// the command table itself
// extern  struct  cmd_type        cmd_table       [];

// Command functions.
// Defined in act_*.c (mostly).

// DECLARE_DO_FUN( do_game         );//RP channel (C) Udun, renamed to GAME (C)Ast
DECLARE_DO_FUN( do_pseudoname   ); // APC - pseudoname
DECLARE_DO_FUN( do_advance      ); // upgraded by Saboteur
DECLARE_DO_FUN( do_affects      ); // upgraded by Saboteur
DECLARE_DO_FUN( do_afk          );
DECLARE_DO_FUN( do_alias        ); // upgraded by Saboteur
DECLARE_DO_FUN( do_allow        ); // rewritten by Saboteur
DECLARE_DO_FUN( do_answer       );
DECLARE_DO_FUN( do_areas        );
DECLARE_DO_FUN( do_at           );
DECLARE_DO_FUN( do_auction      ); // Auction (C) Saboteur
DECLARE_DO_FUN( do_oauction     ); // old auction channel command
DECLARE_DO_FUN( do_autoassist   );
DECLARE_DO_FUN( do_autogold     );
DECLARE_DO_FUN( do_autolist     );
DECLARE_DO_FUN( do_autoloot     );
DECLARE_DO_FUN( do_autosac      );
DECLARE_DO_FUN( do_backstab     );
DECLARE_DO_FUN( do_backup       ); // backup pfiles(C) Adron & Saboteur
DECLARE_DO_FUN( do_backup2      ); // backup pfiles(C) Saboteur
DECLARE_DO_FUN( do_bamfin       );
DECLARE_DO_FUN( do_bamfout      );
DECLARE_DO_FUN( do_ban          ); // rewritten by Saboteur
DECLARE_DO_FUN( do_bash         ); // upgrade by Wagner
DECLARE_DO_FUN( do_blink        ); // blink skill for mages (c)Apc
DECLARE_DO_FUN( do_tail         ); // tail skill  (C) Adron
DECLARE_DO_FUN( do_crush        ); // crush skill (C) Adron
DECLARE_DO_FUN( do_throw        ); // throw skill (C) Adron
DECLARE_DO_FUN( do_berserk      );
DECLARE_DO_FUN( do_brandish     );
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN( do_buy          );
DECLARE_DO_FUN( do_cast         );
DECLARE_DO_FUN( do_changes      );
DECLARE_DO_FUN( do_channels     ); // channel configuration (C) Saboteur
DECLARE_DO_FUN( do_cleave       ); // cleave shield by Thor fixed (C) Astellar
DECLARE_DO_FUN( do_clone        );
DECLARE_DO_FUN( do_close        );
DECLARE_DO_FUN( do_colour       ); // Colour Command By Lope
DECLARE_DO_FUN( do_commands     );
DECLARE_DO_FUN( do_compare      );
DECLARE_DO_FUN( do_consider     );
DECLARE_DO_FUN( do_count        );
DECLARE_DO_FUN( do_credits      );
DECLARE_DO_FUN( do_delete       );
DECLARE_DO_FUN( do_deny         );
DECLARE_DO_FUN( do_description  );
DECLARE_DO_FUN( do_dirt         );
DECLARE_DO_FUN( do_disarm       );
DECLARE_DO_FUN( do_disconnect   );
DECLARE_DO_FUN( do_down         );
DECLARE_DO_FUN( do_drink        ); // updated by Wagner
DECLARE_DO_FUN( do_drop         );
DECLARE_DO_FUN( do_dump         );
DECLARE_DO_FUN( do_east         );
DECLARE_DO_FUN( do_eat          ); // updated by Wagner
DECLARE_DO_FUN( do_echo         );
DECLARE_DO_FUN( do_emote        );
DECLARE_DO_FUN( do_enter        );
DECLARE_DO_FUN( do_check        );
DECLARE_DO_FUN( do_envenom      );
DECLARE_DO_FUN( do_equipment    ); // rewritten by Adron
DECLARE_DO_FUN( do_examine      );
DECLARE_DO_FUN( do_exits        );
DECLARE_DO_FUN( do_fill         );
DECLARE_DO_FUN( do_flag         );
DECLARE_DO_FUN( do_flee         ); // rewritten by Saboteur
DECLARE_DO_FUN( do_follow       );
DECLARE_DO_FUN( do_force        );
DECLARE_DO_FUN( do_frame        );
DECLARE_DO_FUN( do_freeze       );
DECLARE_DO_FUN( do_gain         );
DECLARE_DO_FUN( do_get          );
DECLARE_DO_FUN( do_exchange     );
DECLARE_DO_FUN( do_give         );
DECLARE_DO_FUN( do_send         ); // send item (C) Ak
DECLARE_DO_FUN( do_gossip       );
DECLARE_DO_FUN( do_newbiechat   ); // newbie channel (C) Adron
DECLARE_DO_FUN( do_goto         );
DECLARE_DO_FUN( do_grats        );
DECLARE_DO_FUN( do_group        );
DECLARE_DO_FUN( do_groups       );
DECLARE_DO_FUN( do_gtell        );
DECLARE_DO_FUN( do_guild        );
DECLARE_DO_FUN( do_heal         );
DECLARE_DO_FUN( do_help         );
DECLARE_DO_FUN( do_hide         );
DECLARE_DO_FUN( do_holylight    );
DECLARE_DO_FUN( do_idea         );
DECLARE_DO_FUN( do_immtalk      ); // immortal channel (C) Saboteur
DECLARE_DO_FUN( do_incognito    );
DECLARE_DO_FUN( do_clantalk     ); // clan channel (C) Saboteur
DECLARE_DO_FUN( do_inventory    );
DECLARE_DO_FUN( do_invis        );
DECLARE_DO_FUN( do_kick         ); // upgraded by Saboteur + Wagner
DECLARE_DO_FUN( do_kill         );
DECLARE_DO_FUN( do_list         );
DECLARE_DO_FUN( do_load         );
DECLARE_DO_FUN( do_lock         );
DECLARE_DO_FUN( do_log          );
DECLARE_DO_FUN( do_look         );
DECLARE_DO_FUN( do_memory       );
DECLARE_DO_FUN( do_mfind        );
DECLARE_DO_FUN( do_mload        );
DECLARE_DO_FUN( do_mset         ); // set for npcs (fixed by) Astellar
DECLARE_DO_FUN( do_cset         ); // set for pcs (fixed by) Astellar
DECLARE_DO_FUN( do_mstat        );
DECLARE_DO_FUN( do_affstat      ); // affect statistic (C) Saboteur
DECLARE_DO_FUN( do_mwhere       );
DECLARE_DO_FUN( do_motd         );
DECLARE_DO_FUN( do_murder       );
DECLARE_DO_FUN( do_music        );
DECLARE_DO_FUN( do_news         );
DECLARE_DO_FUN( do_nochannels   );
DECLARE_DO_FUN( do_noemote      );
DECLARE_DO_FUN( do_nofollow     );
DECLARE_DO_FUN( do_north        );
DECLARE_DO_FUN( do_nosummon     );
DECLARE_DO_FUN( do_nocancel     ); // nocancel command (C) Saboteur
DECLARE_DO_FUN( do_nosend       ); // nosend command (C) Saboteur
DECLARE_DO_FUN( do_note         );
DECLARE_DO_FUN( do_offtopic     );//  (C) Udun
DECLARE_DO_FUN( do_notell       );
DECLARE_DO_FUN( do_ofind        );
DECLARE_DO_FUN( do_oload        );
DECLARE_DO_FUN( do_open         );
DECLARE_DO_FUN( do_order        );
DECLARE_DO_FUN( do_oset         );
DECLARE_DO_FUN( do_ostat        );
DECLARE_DO_FUN( do_outfit       );
DECLARE_DO_FUN( do_owhere       );
DECLARE_DO_FUN( do_pardon       );
DECLARE_DO_FUN( do_password     );
DECLARE_DO_FUN( do_peace        );
DECLARE_DO_FUN( do_pecho        );
DECLARE_DO_FUN( do_penalty      );
DECLARE_DO_FUN( do_mob          );
DECLARE_DO_FUN( do_mpstat       );
DECLARE_DO_FUN( do_mpdump       );
DECLARE_DO_FUN( do_surrender    );
DECLARE_DO_FUN( do_pick         );
DECLARE_DO_FUN( do_play         );
DECLARE_DO_FUN( do_pmote        );
DECLARE_DO_FUN( do_pour         );
DECLARE_DO_FUN( do_practice     );
DECLARE_DO_FUN( do_showprac     ); // show practices (C) Saboteur
DECLARE_DO_FUN( do_prefix       );
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN( do_protect      );
DECLARE_DO_FUN( do_purge        ); // rewritten by Saboteur
DECLARE_DO_FUN( do_put          );
DECLARE_DO_FUN( do_quaff        );
DECLARE_DO_FUN( do_question     );
DECLARE_DO_FUN( do_quit         );
DECLARE_DO_FUN( do_quote        );
DECLARE_DO_FUN( do_read         );
DECLARE_DO_FUN( do_reboot       ); // rewritten and improved (C) Saboteur
DECLARE_DO_FUN( do_recall       );
DECLARE_DO_FUN( do_recho        );
DECLARE_DO_FUN( do_recite       );
DECLARE_DO_FUN( do_remove       );
DECLARE_DO_FUN( do_replay       );
DECLARE_DO_FUN( do_reply        );
DECLARE_DO_FUN( do_report       );
DECLARE_DO_FUN( do_rescue       );
DECLARE_DO_FUN( do_rest         );
DECLARE_DO_FUN( do_restore      );
DECLARE_DO_FUN( do_return       );
DECLARE_DO_FUN( do_rset         );
DECLARE_DO_FUN( do_rstat        );
DECLARE_DO_FUN( do_rules        );
DECLARE_DO_FUN( do_sacrifice    );
DECLARE_DO_FUN( do_save         );
DECLARE_DO_FUN( do_say          );
DECLARE_DO_FUN( do_scan         ); // scan (C) Saboteur
DECLARE_DO_FUN( do_score        ); // new score (C) Saboteur
DECLARE_DO_FUN( do_oscore       ); // old score command
DECLARE_DO_FUN( do_online       ); // show online chars (C) Saboteur
DECLARE_DO_FUN( do_scroll       );
DECLARE_DO_FUN( do_sell         );
DECLARE_DO_FUN( do_set          );
DECLARE_DO_FUN( do_shout        );
DECLARE_DO_FUN( do_sedit        ); // edit social online (C) Adron
DECLARE_DO_FUN( do_sit          );
DECLARE_DO_FUN( do_skills       );
DECLARE_DO_FUN( do_slay         );
DECLARE_DO_FUN( do_sleep        );
DECLARE_DO_FUN( do_slookup      );
DECLARE_DO_FUN( do_smote        );
DECLARE_DO_FUN( do_sneak        );
DECLARE_DO_FUN( do_snoop        );
DECLARE_DO_FUN( do_socials      );
DECLARE_DO_FUN( do_south        );
DECLARE_DO_FUN( do_sockets      ); // new socket (C) Saboteur
DECLARE_DO_FUN( do_spells       );
DECLARE_DO_FUN( do_split        );
DECLARE_DO_FUN( do_sset         );
DECLARE_DO_FUN( do_stand        );
DECLARE_DO_FUN( do_stat         );
DECLARE_DO_FUN( do_steal        );
DECLARE_DO_FUN( do_string       );
DECLARE_DO_FUN( do_tell         );
DECLARE_DO_FUN( do_time         ); // rewritten by Saboteur
DECLARE_DO_FUN( do_title        );
DECLARE_DO_FUN( do_tipsy        ); // tipsy by Dinger
DECLARE_DO_FUN( do_train        );
DECLARE_DO_FUN( do_transfer     );
DECLARE_DO_FUN( do_trip         );
DECLARE_DO_FUN( do_trust        );
DECLARE_DO_FUN( do_typo         );
DECLARE_DO_FUN( do_unalias      );
DECLARE_DO_FUN( do_unlock       );
DECLARE_DO_FUN( do_unread       );
DECLARE_DO_FUN( do_up           );
DECLARE_DO_FUN( do_value        );
DECLARE_DO_FUN( do_visible      );
DECLARE_DO_FUN( do_vnum         );
DECLARE_DO_FUN( do_wake         );
DECLARE_DO_FUN( do_wear         );
DECLARE_DO_FUN( do_weather      );
DECLARE_DO_FUN( do_west         );
DECLARE_DO_FUN( do_where        );
DECLARE_DO_FUN( do_who          );
DECLARE_DO_FUN( do_whois        );
DECLARE_DO_FUN( do_wimpy        );
DECLARE_DO_FUN( do_wizhelp      );
DECLARE_DO_FUN( do_wizlist      );
DECLARE_DO_FUN( do_wiznet       ); // Rewritten (C) Saboteur
DECLARE_DO_FUN( do_worth        );
DECLARE_DO_FUN( do_yell         );
DECLARE_DO_FUN( do_zap          );
DECLARE_DO_FUN( do_zecho        );
DECLARE_DO_FUN( do_olc          );
DECLARE_DO_FUN( do_asave        );
DECLARE_DO_FUN( do_alist        );
DECLARE_DO_FUN( do_resets       );
DECLARE_DO_FUN( do_redit        );
DECLARE_DO_FUN( do_aedit        );
DECLARE_DO_FUN( do_medit        );
DECLARE_DO_FUN( do_oedit        );
DECLARE_DO_FUN( do_quest        ); // quest module (C) Saboteur
DECLARE_DO_FUN( do_qstat        ); // quest statistic (C) Saboteur
DECLARE_DO_FUN( do_spellstat    ); // spells statistic (C) Saboteur
DECLARE_DO_FUN( do_skillstat    ); // skills statistic (C) Saboteur
DECLARE_DO_FUN( do_travel       ); // traveller module (C) Saboteur
DECLARE_DO_FUN( do_clan_recall  ); // clan recal (C) Saboteur
DECLARE_DO_FUN( do_arecall      ); // arena and arena recal (C) Saboteur
DECLARE_DO_FUN( do_quenia       ); // quenia skill (C) Saboteur
DECLARE_DO_FUN( do_petition     ); // petition command (C) Saboteur
DECLARE_DO_FUN( do_cleader      ); // set clan leader (C) Saboteur
DECLARE_DO_FUN( do_cfirst       ); // set clan first (C) Black
DECLARE_DO_FUN( do_addlag       ); // add lag (C) Saboteur
DECLARE_DO_FUN( do_nuke         ); // nuke pets (C) Saboteur
DECLARE_DO_FUN( do_remort       ); // remort module (C) Saboteur
DECLARE_DO_FUN( do_bounty       ); // bounty command (C) Saboteur
DECLARE_DO_FUN( do_itemlist     ); // itemlist command (C) Saboteur
DECLARE_DO_FUN( do_mpsave       ); // Save mobprograms (C) Saboteur
DECLARE_DO_FUN( do_mpedit       );
DECLARE_DO_FUN( do_file         );
DECLARE_DO_FUN( do_mplist       );
DECLARE_DO_FUN( do_vlist        ); // vlist command (C) Saboteur
DECLARE_DO_FUN( do_lore         ); // lore skill (C) Saboteur
DECLARE_DO_FUN( do_rename       ); // rename command (C) Saboteur
DECLARE_DO_FUN( do_reform       ); // morph module (C) Adron
DECLARE_DO_FUN( do_polymorph    ); // morph module (C) Adron 
DECLARE_DO_FUN( do_seen         ); // Seen by Cortney. rewritten by Saboteur
DECLARE_DO_FUN( do_test         ); // temporary command for test something
DECLARE_DO_FUN( do_gsocial      ); // Global Socials (C) Sab
DECLARE_DO_FUN( do_config       ); // Configuration command (C) Sab
DECLARE_DO_FUN( do_nomlove      ); // Norape penalty (C)Sab
DECLARE_DO_FUN( do_spec         ); // Raceskill  (C) Sab
DECLARE_DO_FUN( do_showskill    ); // Showskill option for whois (C) Sab
DECLARE_DO_FUN( do_rape         ); // moved from social to command (C) Sab
DECLARE_DO_FUN( do_crimereport  ); // (C) Adron
DECLARE_DO_FUN( do_chat         ); // Chat channel (C) Sab
DECLARE_DO_FUN( do_version      ); // Shows version (info about compiling)
DECLARE_DO_FUN( do_stopfoll     ); // Remove followers from group (C)Sab
DECLARE_DO_FUN( do_offline      ); // for offline char modifications (C) Sab
DECLARE_DO_FUN( do_account      ); // bank for gold (C) Saboteur
DECLARE_DO_FUN( do_clanwork     ); // clanwork for immortals (C) Saboteur
DECLARE_DO_FUN( do_leader       ); // offline leader command (C) Saboteur
DECLARE_DO_FUN( do_blacksmith   ); // Dwarves Blacksmith command (C) Saboteur
DECLARE_DO_FUN( do_ahelp        ); // Advanced help (C) Saboteur
DECLARE_DO_FUN( do_suicide      ); // Suicide command (C) Saboteur
DECLARE_DO_FUN( do_nodelete     ); // nodelete flag (C) Saboteur
DECLARE_DO_FUN( do_nogsocial    ); // nogsocial flag (C) Saboteur
DECLARE_DO_FUN( do_global       ); // global functions (C) Saboteur
DECLARE_DO_FUN( do_diplomacy    ); // Diplomacy for clans (C) Saboteur
DECLARE_DO_FUN( do_alli         ); // Alli channel (C) Saboteur
DECLARE_DO_FUN( do_room         ); // Room god commands (C) Saboteur
DECLARE_DO_FUN( do_pray         ); // Clerics special (C) Jasana
DECLARE_DO_FUN( do_referi       ); // Redery for real arena combat (C) Saboteur
DECLARE_DO_FUN( do_clanfit      ); // making clanenchants (C) Jasana
DECLARE_DO_FUN( do_bugreport    ); // bugreport note type (C) Saboteur
DECLARE_DO_FUN( do_spam         ); // spam config (C) Saboteur
DECLARE_DO_FUN( do_run          ); // Run command (C) Saboteur
DECLARE_DO_FUN( do_clanbank     ); // Clanbank command (C) Saboteur
DECLARE_DO_FUN( do_kazad        ); // Dwarves channel (C) Saboteur
DECLARE_DO_FUN( do_avenge       ); // Assasins' guild channel (C) Astellar
DECLARE_DO_FUN( do_gaccount     ); // Guilds account (C) Saboteur
DECLARE_DO_FUN( do_moblist      ); // List of Mobs - areabuilding (C) Saboteur
DECLARE_DO_FUN( do_add_penalty  ); // Penalties with timer (beta) (C) Saboteur
DECLARE_DO_FUN( do_vote         ); // Voting (C) Saboteur
DECLARE_DO_FUN( do_offer        ); // Offer for assasin guild (C) Saboteur
DECLARE_DO_FUN( do_shock_hit    ); // shock hit for thieves (C) Saboteur
DECLARE_DO_FUN( do_inform       ); // look cast bonus and groups (C) Saboteur
DECLARE_DO_FUN( do_setclass     ); // set class history (2imms) (C) Saboteur
DECLARE_DO_FUN( do_marry        ); // Marry (C) Saboteur
DECLARE_DO_FUN( do_ptalk        ); // Channel for married (C) Saboteur
DECLARE_DO_FUN( do_divorce      ); // Divorce (C) Saboteur
DECLARE_DO_FUN( do_vislist      ); // Visible List (C) Saboteur
DECLARE_DO_FUN( do_ignorelist   ); // Ignore  List (C) Apc :) 
DECLARE_DO_FUN( do_censored     ); // Censored channel (C) Saboteur
DECLARE_DO_FUN( do_sprecall     ); // Recall for marryed (C) Saboteur
DECLARE_DO_FUN( do_nopost       ); // NOPOST (penalty) (C) Saboteur
DECLARE_DO_FUN( do_race         ); // RACE table work/edit (C) Saboteur
DECLARE_DO_FUN( do_walk         ); // Приземлится (C) Saboteur
DECLARE_DO_FUN( do_fly          ); // Взлететь (race special) (C) Saboteur
DECLARE_DO_FUN( do_smoke        ); // типа социала (C) Saboteur
DECLARE_DO_FUN( do_dampool      ); // Просмотр Dampool (C) Saboteur
DECLARE_DO_FUN( do_newspaper    ); // Газета (C) Saboteur
DECLARE_DO_FUN( do_russian      ); // Русский язык (C) Saboteur
DECLARE_DO_FUN( do_tournament   ); // Tournaments (C) Imperror
DECLARE_DO_FUN( do_talk         ); // save talks in lostlink (C) Saboteur
DECLARE_DO_FUN( do_deaf         ); // режим глухоты (C) Saboteur (каналы)
DECLARE_DO_FUN( do_quiet        ); // режим тишины (C) Saboteur (разговоры)
DECLARE_DO_FUN( do_pose         ); // poses
DECLARE_DO_FUN( do_vbite        ); // Vampires Bite (C) Saboteur (guild skill)
DECLARE_DO_FUN( do_mist         ); // Vampires MIST (C) Saboteur
DECLARE_DO_FUN( do_howl         ); // Ethereals Howl (C) Saboteur
DECLARE_DO_FUN( do_charge       ); // Spell charge (C) Imperror
DECLARE_DO_FUN( do_setcurse     ); // Set God Curse (C) Magic
DECLARE_DO_FUN( do_gquest       ); // Global Quest (C) Saboteur
DECLARE_DO_FUN( do_lash         ); // Lash skill (C) Saboteur
DECLARE_DO_FUN( do_dig          ); // prikol (C) Saboteur
DECLARE_DO_FUN( do_strangle     ); // Thief skill (C) Gendalf
DECLARE_DO_FUN( do_resurrect    ); // Druids Guild Feature (C) Saboteur
DECLARE_DO_FUN( do_rlist        ); // Rooms list (C) Saboteur
DECLARE_DO_FUN( do_srcwrite     ); // My personal special functions (C) Saboteur
DECLARE_DO_FUN( do_family       ); // Family Immortal Utility (C) Saboteur
DECLARE_DO_FUN( do_pban         ); // Personal Ban (C) Saboteur
DECLARE_DO_FUN( do_pacify       ); // Pacify aggrs as Orden clanskill (C) Apc
DECLARE_DO_FUN( do_player       ); // Immortal test command (C) Saboteur
DECLARE_DO_FUN( do_push         ); // Push objects (C) Saboteur
DECLARE_DO_FUN( do_repair       ); // Item repairing (C) Astellar
DECLARE_DO_FUN( do_reward       ); // Honour players with reward (C) Astellar
DECLARE_DO_FUN( do_index        ); // Immortal command for listing mobiles
DECLARE_DO_FUN( do_damage       ); // Damages items (imms') (C) Astellar
DECLARE_DO_FUN( do_seize        ); // Confiscate items (C) Astellar
DECLARE_DO_FUN( do_devote       ); // Choosing a Deity (C) Astellar
DECLARE_DO_FUN( do_deity        ); // Editing Deities  (C) Astellar
DECLARE_DO_FUN( do_polyanarecall); // Polyanrecall  (C) Astellar
DECLARE_DO_FUN( do_cfix         ); // fix characters  (C) Astellar
DECLARE_DO_FUN( do_gfix         ); // global fix conception command(C) Astellar
DECLARE_DO_FUN( do_damage_obj   ); // damaging object  (C) Astellar
// DECLARE_DO_FUN( do_supplicate   ); // supplicate to the deity (C) Astellar
DECLARE_DO_FUN( do_forge        ); // forge items (C) Astellar
DECLARE_DO_FUN( do_ldefend      ); // level defend character (C) Astellar
DECLARE_DO_FUN( do_complain     ); // complains of players (C) Apc
