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

// DECLARE_DO_FUN2( do_game         );//RP channel (C) Udun, renamed to GAME (C)Ast
DECLARE_DO_FUN2( do_pseudoname   ); // APC - pseudoname
DECLARE_DO_FUN2( do_advance      ); // upgraded by Saboteur
DECLARE_DO_FUN2( do_affects      ); // upgraded by Saboteur
DECLARE_DO_FUN2( do_afk          );
DECLARE_DO_FUN2( do_alias        ); // upgraded by Saboteur
DECLARE_DO_FUN2( do_allow        ); // rewritten by Saboteur
DECLARE_DO_FUN2( do_answer       );
DECLARE_DO_FUN2( do_areas        );
DECLARE_DO_FUN2( do_at           );
DECLARE_DO_FUN2( do_auction      ); // Auction (C) Saboteur
DECLARE_DO_FUN2( do_oauction     ); // old auction channel command
DECLARE_DO_FUN2( do_autoassist   );
DECLARE_DO_FUN2( do_autogold     );
DECLARE_DO_FUN2( do_autolist     );
DECLARE_DO_FUN2( do_autoloot     );
DECLARE_DO_FUN2( do_autosac      );
DECLARE_DO_FUN2( do_backstab     );
DECLARE_DO_FUN2( do_backup       ); // backup pfiles(C) Adron & Saboteur
DECLARE_DO_FUN2( do_backup2      ); // backup pfiles(C) Saboteur
DECLARE_DO_FUN2( do_bamfin       );
DECLARE_DO_FUN2( do_bamfout      );
DECLARE_DO_FUN2( do_ban          ); // rewritten by Saboteur
DECLARE_DO_FUN2( do_bash         ); // upgrade by Wagner
DECLARE_DO_FUN2( do_blink        ); // blink skill for mages (c)Apc
DECLARE_DO_FUN2( do_tail         ); // tail skill  (C) Adron
DECLARE_DO_FUN2( do_crush        ); // crush skill (C) Adron
DECLARE_DO_FUN2( do_throw        ); // throw skill (C) Adron
DECLARE_DO_FUN2( do_berserk      );
DECLARE_DO_FUN2( do_brandish     );
DECLARE_DO_FUN2( do_brief        );
DECLARE_DO_FUN2( do_buy          );
DECLARE_DO_FUN2( do_cast         );
DECLARE_DO_FUN2( do_changes      );
DECLARE_DO_FUN2( do_channels     ); // channel configuration (C) Saboteur
DECLARE_DO_FUN2( do_cleave       ); // cleave shield by Thor fixed (C) Astellar
DECLARE_DO_FUN2( do_clone        );
DECLARE_DO_FUN2( do_close        );
DECLARE_DO_FUN2( do_colour       ); // Colour Command By Lope
DECLARE_DO_FUN2( do_commands     );
DECLARE_DO_FUN2( do_compare      );
DECLARE_DO_FUN2( do_consider     );
DECLARE_DO_FUN2( do_count        );
DECLARE_DO_FUN2( do_credits      );
DECLARE_DO_FUN2( do_delete       );
DECLARE_DO_FUN2( do_deny         );
DECLARE_DO_FUN2( do_description  );
DECLARE_DO_FUN2( do_dirt         );
DECLARE_DO_FUN2( do_disarm       );
DECLARE_DO_FUN2( do_disconnect   );
DECLARE_DO_FUN2( do_down         );
DECLARE_DO_FUN2( do_drink        ); // updated by Wagner
DECLARE_DO_FUN2( do_drop         );
DECLARE_DO_FUN2( do_dump         );
DECLARE_DO_FUN2( do_east         );
DECLARE_DO_FUN2( do_eat          ); // updated by Wagner
DECLARE_DO_FUN2( do_echo         );
DECLARE_DO_FUN2( do_emote        );
DECLARE_DO_FUN2( do_enter        );
DECLARE_DO_FUN2( do_check        );
DECLARE_DO_FUN2( do_envenom      );
DECLARE_DO_FUN2( do_equipment    ); // rewritten by Adron
DECLARE_DO_FUN2( do_examine      );
DECLARE_DO_FUN2( do_exits        );
DECLARE_DO_FUN2( do_fill         );
DECLARE_DO_FUN2( do_flag         );
DECLARE_DO_FUN2( do_flee         ); // rewritten by Saboteur
DECLARE_DO_FUN2( do_follow       );
DECLARE_DO_FUN2( do_force        );
DECLARE_DO_FUN2( do_frame        );
DECLARE_DO_FUN2( do_freeze       );
DECLARE_DO_FUN2( do_gain         );
DECLARE_DO_FUN2( do_get          );
DECLARE_DO_FUN2( do_exchange     );
DECLARE_DO_FUN2( do_give         );
DECLARE_DO_FUN2( do_send         ); // send item (C) Ak
DECLARE_DO_FUN2( do_gossip       );
DECLARE_DO_FUN2( do_newbiechat   ); // newbie channel (C) Adron
DECLARE_DO_FUN2( do_goto         );
DECLARE_DO_FUN2( do_grats        );
DECLARE_DO_FUN2( do_group        );
DECLARE_DO_FUN2( do_groups       );
DECLARE_DO_FUN2( do_gtell        );
DECLARE_DO_FUN2( do_guild        );
DECLARE_DO_FUN2( do_heal         );
DECLARE_DO_FUN2( do_help         );
DECLARE_DO_FUN2( do_hide         );
DECLARE_DO_FUN2( do_holylight    );
DECLARE_DO_FUN2( do_idea         );
DECLARE_DO_FUN2( do_immtalk      ); // immortal channel (C) Saboteur
DECLARE_DO_FUN2( do_incognito    );
DECLARE_DO_FUN2( do_clantalk     ); // clan channel (C) Saboteur
DECLARE_DO_FUN2( do_inventory    );
DECLARE_DO_FUN2( do_invis        );
DECLARE_DO_FUN2( do_kick         ); // upgraded by Saboteur + Wagner
DECLARE_DO_FUN2( do_kill         );
DECLARE_DO_FUN2( do_list         );
DECLARE_DO_FUN2( do_load         );
DECLARE_DO_FUN2( do_lock         );
DECLARE_DO_FUN2( do_log          );
DECLARE_DO_FUN2( do_look         );
DECLARE_DO_FUN2( do_memory       );
DECLARE_DO_FUN2( do_mfind        );
DECLARE_DO_FUN2( do_mload        );
DECLARE_DO_FUN2( do_mset         ); // set for npcs (fixed by) Astellar
DECLARE_DO_FUN2( do_cset         ); // set for pcs (fixed by) Astellar
DECLARE_DO_FUN2( do_mstat        );
DECLARE_DO_FUN2( do_affstat      ); // affect statistic (C) Saboteur
DECLARE_DO_FUN2( do_mwhere       );
DECLARE_DO_FUN2( do_motd         );
DECLARE_DO_FUN2( do_murder       );
DECLARE_DO_FUN2( do_music        );
DECLARE_DO_FUN2( do_news         );
DECLARE_DO_FUN2( do_nochannels   );
DECLARE_DO_FUN2( do_noemote      );
DECLARE_DO_FUN2( do_nofollow     );
DECLARE_DO_FUN2( do_north        );
DECLARE_DO_FUN2( do_nosummon     );
DECLARE_DO_FUN2( do_nocancel     ); // nocancel command (C) Saboteur
DECLARE_DO_FUN2( do_nosend       ); // nosend command (C) Saboteur
DECLARE_DO_FUN2( do_note         );
DECLARE_DO_FUN2( do_offtopic     );//  (C) Udun
DECLARE_DO_FUN2( do_notell       );
DECLARE_DO_FUN2( do_ofind        );
DECLARE_DO_FUN2( do_oload        );
DECLARE_DO_FUN2( do_open         );
DECLARE_DO_FUN2( do_order        );
DECLARE_DO_FUN2( do_oset         );
DECLARE_DO_FUN2( do_ostat        );
DECLARE_DO_FUN2( do_outfit       );
DECLARE_DO_FUN2( do_owhere       );
DECLARE_DO_FUN2( do_pardon       );
DECLARE_DO_FUN2( do_password     );
DECLARE_DO_FUN2( do_peace        );
DECLARE_DO_FUN2( do_pecho        );
DECLARE_DO_FUN2( do_penalty      );
DECLARE_DO_FUN2( do_mob          );
DECLARE_DO_FUN2( do_mpstat       );
DECLARE_DO_FUN2( do_mpdump       );
DECLARE_DO_FUN2( do_surrender    );
DECLARE_DO_FUN2( do_pick         );
DECLARE_DO_FUN2( do_play         );
DECLARE_DO_FUN2( do_pmote        );
DECLARE_DO_FUN2( do_pour         );
DECLARE_DO_FUN2( do_practice     );
DECLARE_DO_FUN2( do_showprac     ); // show practices (C) Saboteur
DECLARE_DO_FUN2( do_prefix       );
DECLARE_DO_FUN2( do_prompt       );
DECLARE_DO_FUN2( do_protect      );
DECLARE_DO_FUN2( do_purge        ); // rewritten by Saboteur
DECLARE_DO_FUN2( do_put          );
DECLARE_DO_FUN2( do_quaff        );
DECLARE_DO_FUN2( do_question     );
DECLARE_DO_FUN2( do_quit         );
DECLARE_DO_FUN2( do_quote        );
DECLARE_DO_FUN2( do_read         );
DECLARE_DO_FUN2( do_reboot       ); // rewritten and improved (C) Saboteur
DECLARE_DO_FUN2( do_recall       );
DECLARE_DO_FUN2( do_recho        );
DECLARE_DO_FUN2( do_recite       );
DECLARE_DO_FUN2( do_remove       );
DECLARE_DO_FUN2( do_replay       );
DECLARE_DO_FUN2( do_reply        );
DECLARE_DO_FUN2( do_report       );
DECLARE_DO_FUN2( do_rescue       );
DECLARE_DO_FUN2( do_rest         );
DECLARE_DO_FUN2( do_restore      );
DECLARE_DO_FUN2( do_return       );
DECLARE_DO_FUN2( do_rset         );
DECLARE_DO_FUN2( do_rstat        );
DECLARE_DO_FUN2( do_rules        );
DECLARE_DO_FUN2( do_sacrifice    );
DECLARE_DO_FUN2( do_save         );
DECLARE_DO_FUN2( do_say          );
DECLARE_DO_FUN2( do_scan         ); // scan (C) Saboteur
DECLARE_DO_FUN2( do_score        ); // new score (C) Saboteur
DECLARE_DO_FUN2( do_oscore       ); // old score command
DECLARE_DO_FUN2( do_online       ); // show online chars (C) Saboteur
DECLARE_DO_FUN2( do_scroll       );
DECLARE_DO_FUN2( do_sell         );
DECLARE_DO_FUN2( do_set          );
DECLARE_DO_FUN2( do_shout        );
DECLARE_DO_FUN2( do_sedit        ); // edit social online (C) Adron
DECLARE_DO_FUN2( do_sit          );
DECLARE_DO_FUN2( do_skills       );
DECLARE_DO_FUN2( do_slay         );
DECLARE_DO_FUN2( do_sleep        );
DECLARE_DO_FUN2( do_slookup      );
DECLARE_DO_FUN2( do_smote        );
DECLARE_DO_FUN2( do_sneak        );
DECLARE_DO_FUN2( do_snoop        );
DECLARE_DO_FUN2( do_socials      );
DECLARE_DO_FUN2( do_south        );
DECLARE_DO_FUN2( do_sockets      ); // new socket (C) Saboteur
DECLARE_DO_FUN2( do_spells       );
DECLARE_DO_FUN2( do_split        );
DECLARE_DO_FUN2( do_sset         );
DECLARE_DO_FUN2( do_stand        );
DECLARE_DO_FUN2( do_stat         );
DECLARE_DO_FUN2( do_steal        );
DECLARE_DO_FUN2( do_string       );
DECLARE_DO_FUN2( do_tell         );
DECLARE_DO_FUN2( do_time         ); // rewritten by Saboteur
DECLARE_DO_FUN2( do_title        );
DECLARE_DO_FUN2( do_tipsy        ); // tipsy by Dinger
DECLARE_DO_FUN2( do_train        );
DECLARE_DO_FUN2( do_transfer     );
DECLARE_DO_FUN2( do_trip         );
DECLARE_DO_FUN2( do_trust        );
DECLARE_DO_FUN2( do_typo         );
DECLARE_DO_FUN2( do_unalias      );
DECLARE_DO_FUN2( do_unlock       );
DECLARE_DO_FUN2( do_unread       );
DECLARE_DO_FUN2( do_up           );
DECLARE_DO_FUN2( do_value        );
DECLARE_DO_FUN2( do_visible      );
DECLARE_DO_FUN2( do_vnum         );
DECLARE_DO_FUN2( do_wake         );
DECLARE_DO_FUN2( do_wear         );
DECLARE_DO_FUN2( do_weather      );
DECLARE_DO_FUN2( do_west         );
DECLARE_DO_FUN2( do_where        );
DECLARE_DO_FUN2( do_who          );
DECLARE_DO_FUN2( do_whois        );
DECLARE_DO_FUN2( do_wimpy        );
DECLARE_DO_FUN2( do_wizhelp      );
DECLARE_DO_FUN2( do_wizlist      );
DECLARE_DO_FUN2( do_wiznet       ); // Rewritten (C) Saboteur
DECLARE_DO_FUN2( do_worth        );
DECLARE_DO_FUN2( do_yell         );
DECLARE_DO_FUN2( do_zap          );
DECLARE_DO_FUN2( do_zecho        );
DECLARE_DO_FUN2( do_olc          );
DECLARE_DO_FUN2( do_asave        );
DECLARE_DO_FUN2( do_alist        );
DECLARE_DO_FUN2( do_resets       );
DECLARE_DO_FUN2( do_redit        );
DECLARE_DO_FUN2( do_aedit        );
DECLARE_DO_FUN2( do_medit        );
DECLARE_DO_FUN2( do_oedit        );
DECLARE_DO_FUN2( do_quest        ); // quest module (C) Saboteur
DECLARE_DO_FUN2( do_qstat        ); // quest statistic (C) Saboteur
DECLARE_DO_FUN2( do_spellstat    ); // spells statistic (C) Saboteur
DECLARE_DO_FUN2( do_skillstat    ); // skills statistic (C) Saboteur
DECLARE_DO_FUN2( do_travel       ); // traveller module (C) Saboteur
DECLARE_DO_FUN2( do_clan_recall  ); // clan recal (C) Saboteur
DECLARE_DO_FUN2( do_arecall      ); // arena and arena recal (C) Saboteur
DECLARE_DO_FUN2( do_quenia       ); // quenia skill (C) Saboteur
DECLARE_DO_FUN2( do_petition     ); // petition command (C) Saboteur
DECLARE_DO_FUN2( do_cleader      ); // set clan leader (C) Saboteur
DECLARE_DO_FUN2( do_cfirst       ); // set clan first (C) Black
DECLARE_DO_FUN2( do_addlag       ); // add lag (C) Saboteur
DECLARE_DO_FUN2( do_nuke         ); // nuke pets (C) Saboteur
DECLARE_DO_FUN2( do_remort       ); // remort module (C) Saboteur
DECLARE_DO_FUN2( do_bounty       ); // bounty command (C) Saboteur
DECLARE_DO_FUN2( do_itemlist     ); // itemlist command (C) Saboteur
DECLARE_DO_FUN2( do_mpsave       ); // Save mobprograms (C) Saboteur
DECLARE_DO_FUN2( do_mpedit       );
DECLARE_DO_FUN2( do_mplist       );
DECLARE_DO_FUN2( do_vlist        ); // vlist command (C) Saboteur
DECLARE_DO_FUN2( do_lore         ); // lore skill (C) Saboteur
DECLARE_DO_FUN2( do_rename       ); // rename command (C) Saboteur
DECLARE_DO_FUN2( do_reform       ); // morph module (C) Adron
DECLARE_DO_FUN2( do_polymorph    ); // morph module (C) Adron 
DECLARE_DO_FUN2( do_seen         ); // Seen by Cortney. rewritten by Saboteur
DECLARE_DO_FUN2( do_test         ); // temporary command for test something
DECLARE_DO_FUN2( do_gsocial      ); // Global Socials (C) Sab
DECLARE_DO_FUN2( do_config       ); // Configuration command (C) Sab
DECLARE_DO_FUN2( do_nomlove      ); // Norape penalty (C)Sab
DECLARE_DO_FUN2( do_spec         ); // Raceskill  (C) Sab
DECLARE_DO_FUN2( do_showskill    ); // Showskill option for whois (C) Sab
DECLARE_DO_FUN2( do_rape         ); // moved from social to command (C) Sab
DECLARE_DO_FUN2( do_crimereport  ); // (C) Adron
DECLARE_DO_FUN2( do_chat         ); // Chat channel (C) Sab
DECLARE_DO_FUN2( do_version      ); // Shows version (info about compiling)
DECLARE_DO_FUN2( do_stopfoll     ); // Remove followers from group (C)Sab
DECLARE_DO_FUN2( do_offline      ); // for offline char modifications (C) Sab
DECLARE_DO_FUN2( do_account      ); // bank for gold (C) Saboteur
DECLARE_DO_FUN2( do_clanwork     ); // clanwork for immortals (C) Saboteur
DECLARE_DO_FUN2( do_leader       ); // offline leader command (C) Saboteur
DECLARE_DO_FUN2( do_blacksmith   ); // Dwarves Blacksmith command (C) Saboteur
DECLARE_DO_FUN2( do_ahelp        ); // Advanced help (C) Saboteur
DECLARE_DO_FUN2( do_suicide      ); // Suicide command (C) Saboteur
DECLARE_DO_FUN2( do_nodelete     ); // nodelete flag (C) Saboteur
DECLARE_DO_FUN2( do_nogsocial    ); // nogsocial flag (C) Saboteur
DECLARE_DO_FUN2( do_global       ); // global functions (C) Saboteur
DECLARE_DO_FUN2( do_diplomacy    ); // Diplomacy for clans (C) Saboteur
DECLARE_DO_FUN2( do_alli         ); // Alli channel (C) Saboteur
DECLARE_DO_FUN2( do_room         ); // Room god commands (C) Saboteur
DECLARE_DO_FUN2( do_pray         ); // Clerics special (C) Jasana
DECLARE_DO_FUN2( do_referi       ); // Redery for real arena combat (C) Saboteur
DECLARE_DO_FUN2( do_clanfit      ); // making clanenchants (C) Jasana
DECLARE_DO_FUN2( do_bugreport    ); // bugreport note type (C) Saboteur
DECLARE_DO_FUN2( do_spam         ); // spam config (C) Saboteur
DECLARE_DO_FUN2( do_run          ); // Run command (C) Saboteur
DECLARE_DO_FUN2( do_clanbank     ); // Clanbank command (C) Saboteur
DECLARE_DO_FUN2( do_kazad        ); // Dwarves channel (C) Saboteur
DECLARE_DO_FUN2( do_avenge       ); // Assasins' guild channel (C) Astellar
DECLARE_DO_FUN2( do_gaccount     ); // Guilds account (C) Saboteur
DECLARE_DO_FUN2( do_moblist      ); // List of Mobs - areabuilding (C) Saboteur
DECLARE_DO_FUN2( do_add_penalty  ); // Penalties with timer (beta) (C) Saboteur
DECLARE_DO_FUN2( do_vote         ); // Voting (C) Saboteur
DECLARE_DO_FUN2( do_offer        ); // Offer for assasin guild (C) Saboteur
DECLARE_DO_FUN2( do_shock_hit    ); // shock hit for thieves (C) Saboteur
DECLARE_DO_FUN2( do_inform       ); // look cast bonus and groups (C) Saboteur
DECLARE_DO_FUN2( do_setclass     ); // set class history (2imms) (C) Saboteur
DECLARE_DO_FUN2( do_marry        ); // Marry (C) Saboteur
DECLARE_DO_FUN2( do_ptalk        ); // Channel for married (C) Saboteur
DECLARE_DO_FUN2( do_divorce      ); // Divorce (C) Saboteur
DECLARE_DO_FUN2( do_vislist      ); // Visible List (C) Saboteur
DECLARE_DO_FUN2( do_ignorelist   ); // Ignore  List (C) Apc :) 
DECLARE_DO_FUN2( do_censored     ); // Censored channel (C) Saboteur
DECLARE_DO_FUN2( do_sprecall     ); // Recall for marryed (C) Saboteur
DECLARE_DO_FUN2( do_nopost       ); // NOPOST (penalty) (C) Saboteur
DECLARE_DO_FUN2( do_race         ); // RACE table work/edit (C) Saboteur
DECLARE_DO_FUN2( do_walk         ); // to land (C) Saboteur
DECLARE_DO_FUN2( do_fly          ); // to fly (race special) (C) Saboteur
DECLARE_DO_FUN2( do_smoke        ); // a social (C) Saboteur
DECLARE_DO_FUN2( do_dampool      ); // View Dampool (C) Saboteur
DECLARE_DO_FUN2( do_newspaper    ); // Newspaper (C) Saboteur
DECLARE_DO_FUN2( do_tournament   ); // Tournaments (C) Imperror
DECLARE_DO_FUN2( do_talk         ); // save talks in lostlink (C) Saboteur
DECLARE_DO_FUN2( do_deaf         ); // deaf mode (C) Saboteur (channels)
DECLARE_DO_FUN2( do_quiet        ); // quiet mode (C) Saboteur (talks)
DECLARE_DO_FUN2( do_pose         ); // poses
DECLARE_DO_FUN2( do_vbite        ); // Vampires Bite (C) Saboteur (guild skill)
DECLARE_DO_FUN2( do_mist         ); // Vampires MIST (C) Saboteur
DECLARE_DO_FUN2( do_howl         ); // Ethereals Howl (C) Saboteur
DECLARE_DO_FUN2( do_charge       ); // Spell charge (C) Imperror
DECLARE_DO_FUN2( do_setcurse     ); // Set God Curse (C) Magic
DECLARE_DO_FUN2( do_gquest       ); // Global Quest (C) Saboteur
DECLARE_DO_FUN2( do_lash         ); // Lash skill (C) Saboteur
DECLARE_DO_FUN2( do_dig          ); // fun action (C) Saboteur
DECLARE_DO_FUN2( do_strangle     ); // Thief skill (C) Gendalf
DECLARE_DO_FUN2( do_resurrect    ); // Druids Guild Feature (C) Saboteur
DECLARE_DO_FUN2( do_rlist        ); // Rooms list (C) Saboteur
DECLARE_DO_FUN2( do_srcwrite     ); // My personal special functions (C) Saboteur
DECLARE_DO_FUN2( do_family       ); // Family Immortal Utility (C) Saboteur
DECLARE_DO_FUN2( do_pban         ); // Personal Ban (C) Saboteur
DECLARE_DO_FUN2( do_pacify       ); // Pacify aggrs as Orden clanskill (C) Apc
DECLARE_DO_FUN2( do_player       ); // Immortal test command (C) Saboteur
DECLARE_DO_FUN2( do_push         ); // Push objects (C) Saboteur
DECLARE_DO_FUN2( do_repair       ); // Item repairing (C) Astellar
DECLARE_DO_FUN2( do_reward       ); // Honour players with reward (C) Astellar
DECLARE_DO_FUN2( do_index        ); // Immortal command for listing mobiles
DECLARE_DO_FUN2( do_damage       ); // Damages items (imms') (C) Astellar
DECLARE_DO_FUN2( do_seize        ); // Confiscate items (C) Astellar
DECLARE_DO_FUN2( do_devote       ); // Choosing a Deity (C) Astellar
DECLARE_DO_FUN2( do_deity        ); // Editing Deities  (C) Astellar
DECLARE_DO_FUN2( do_polyanarecall); // Polyanrecall  (C) Astellar
DECLARE_DO_FUN2( do_cfix         ); // fix characters  (C) Astellar
DECLARE_DO_FUN2( do_gfix         ); // global fix conception command(C) Astellar
DECLARE_DO_FUN2( do_damage_obj   ); // damaging object  (C) Astellar
DECLARE_DO_FUN2( do_forge        ); // forge items (C) Astellar
DECLARE_DO_FUN2( do_ldefend      ); // level defend character (C) Astellar
DECLARE_DO_FUN2( do_complain     ); // complains of players (C) Apc
