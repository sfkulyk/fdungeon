// Copyrights (C) 1998-2003, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'

test of cvs
Invader

his is test file. Haw do you do?
 Saboteur

Nu? chegovse molchat?
 Saboteur

Another Commit from work :)) a vi lameraa! :}}}}}
Saboteur

Test for messages
Saboteur

another and another

Sorry guys for intruding in your repository - but need to test new
commit management framework. lx.

Oops, forgot to update cvs-mudhosix alias to include all of mud
developers.

Hi all.
Anybody - I need to get all area files.. my sources compiled succesfully,
but executable viletaet so slovami Create Object: ObjIndex NULL
v obshem zavtra (V ponedelnik) ia budu v KPI.
Saboteur

--------------------------------------------------------------------------
List of changes:

Initial code version: Rom24b4a (from www.game.org)

Initial version of Forgotten Dungeon was made by Saboteur LocalMode mud.
 - color by Loper
 - OLC for 2.4 implemented
 - quest (idea by Vassago) fully rewritten by Saboteur
   (Thanx to Catalina for beta testing)
 - charset implemented, russian codepages inherited (thanx to Merlin for help)
 - added score command, old score renamed to oscore
 - changed max_level from 51 to 110 in all src

Many changes was lost...
Now, common changes list after installing on hosix.ntu-kpi.kiev.ua 4000
Mud Birthday date: 2.09.1998

 - added %q and %Q options to prompt (to quest, for quest time)
 - deny remove curse on mobs
 - deny cast poison in safe rooms
 - q cancel now removes 5-20 qp
 - function seen by Cortney added and removed (very buggy :)
 - added "note list -xx" format (list last xx notes/penalties...) (C) Saboteur
 - LastLogin added,
   Arenarecal bug fixed
   Imms can recal in anyway
   Cortney
 - Fixed bug with lastlogin (C) Adron
 - Fixed nedofixeniy lastlogin, added lastlogin time (C) Cortney
 - socials now uses get_char_world
 - mlove use get_char_room
 - classes now has personal classname
 - mlt now can train hp/mana
 - newbiechat added
 - new quest items added
 - deny auctioning quest items
 - deny to bounty gods by mortals (C) Invader
 - transliteration fixed (C) Invader
 - personal clanrecall added (C) Cortney
 - added gsocials, added TO_ALL option to act
 - add Autors.c - for credits
 - fixed leaking first symbols under win32 (C) Sab!
 - added Death/Kill statistik (26.may.99)
 - added AID (C) Cortney
 - Parsing added (C) Warlock
 - added startup macros (C) Saboteur
 - socket <name> added (C) Cortney
 - fixed delete-quit xerox bug (C) Cortney, thanx to Magica
 - fixed curse spell in safe (C) Saboteur
 - reboot command improved to work with startup macros (C) Saboteur
 - added config command,added config showkills, whois changed (C) Saboteur

 - Quest statistic added 12/06/1999 (C) Saboteur
 - Quest time: completed quest - tnq 3-10, failed quest - 7-23 ticks.
   Fixed steal from fightings chars.
   Many bugs fixed.
   Many cosmetic fixes.
   Autodamage and autoexit added to config. (C) Saboteur
 - score cosmetic changes
   can_carry fixed (gods now can carry 99999 not 10000000 - for right work)
   Sanctuary now can be refreshed "on fly". (C) Saboteur 
 - Fixed 0d0d0a to 0d0a in few files.
   Cosmetic changes in score
   added do_rape command. mlove social removed.
   reweitten clan.c (petition, make_corpse)
   many changes and warnings
   added fly and showskill command
   (C) Saboteur
 - Channels rewritten - channels on/off by command channels
 - REMORT - Customizing char rewritten
 - Hassan now do not kill rapers, but put them to condition -2 hp. Sab
 - added dispel messages to all spells. Sab
 - nomlove command added
   get_skill_cost rewritten for mlt
   spell armor can be refreshed.
   Saboteur
 - refreshed added to detect hidden, detect invisible, detect good, detect evil
   make corpse rewritten
   do_fly added
   Saboteur
 - added saves info to affect command (C) Invader
 - added rape command, social mlove removed
   classcle, classmag,classthi.classwar, remort, toquest variables added
   ooc renamed to CHAT
   rewritten do_time
 - added do_crush - orcs special. Adron
 - added do_tail - lizards special. Adron
 - new equipment echo by Adron added
   Breathes now damage*2 !
 - char in limbo will not be updated. Sab
 - note to clan, noclan, leader, <name>, <clanname> now works (C) Sab
 - Now pets and charmed mobs report, when they cannot do a command. Adron.
 - winmake rewritten
   mobs now cast with do_cast
   saves spell now in score (20 level).
   Sab
 - Added some new interesting features to DO_EAT, REMOVE_OBJ, WEAR_OBJ.
   Now it's impossible to remove armor, leggings and boots in
   combat, and all other can be removed with waitstate.
   Fans of Warp Pills will curse my name with BAD words... :)))
   Adron
 - gain condition improved - hungry and thirsty chars can die... (C) Saboteur
   added io.c for i/o functions (C) Sab
   COMM flags = int64
   channel command added 
   showprac added
   Saboteur
 - rewritten command table, added flag variable. Sab
   do_affect rewritten
   farsight fixed
   Added HIDE and MORPH flags. Commands with HIDE do not removes HIDE affect
   Some optimizing in note.c
   Fixed bug with exp
   Fixed kill logging message
   Fixed channels. Comm flag are 64bit now.
   Saboteur
 - Added showpractice command.Sab
 - Affects rewritten
   Alist - 102 levl
   added VERSION command (C) Saboteur
  
10 version 1.5
   Thieves now can flee in specified direction.
   Added check size for kick
   Added check dex for dodge
   Added check dex&str for parry
   Added FULL flag to command
   Removed all unusable functions (do_delet, do_qui, do_alia)
   Race Dwarf are MEDIUM_SIZE
   Race Ethereal are SMALL_SIZE
   Race Centaurs has double kick now.
   Added failed phraze to curse spell.
   Fixed channel bukva K.
11 version 1.6
   Fixed scan
   added online command
   added autoonline option
12 version 1.7
   rewritten act_move.c act_enter.c (optimized about to 15 kb)
   rewritten save.c (optimized about to 15 kb)
13 version 1.8
   Adrenalin with mobs fixed.
14 version 1.9
   Fixed online title.
   Version now show to users when them loggin in.
   Rewritten act_info.c about 15 kb free.
   Removed COMPACT and COMBINE flags. (now always on)
   Changed Logging to LOW detail.
   Rewritten interp.c
   Added NOMOB flag to command table (NPC can't use command with NOMOB)
   Fixed portals.

15 version 1.99
   Added stopfoll command (nofol do not removes current followers from group)
16 version 1.991
   Fixed practice
17 version 2.00
   Vampires do not change align by vampiric weapon
   Fixed flee north
   Some optimizing in magic.c Many spells moved to magic2.c
18 Offline command implemented. Sab
   changed loading: mud will continue loading, if cant' find area, signed in
   area.lst (all exits will changed to Temple (Hassan))   
   offline deny added. Sab
19 version 2.02
   Added zritel command
   Added memorizing chars
   Added Delta.are
   ... lost Castle.are
   Fixed MANY bugs
20 Fixed very many bugs
   Fixed afk
   Added anti-xerox feature :]~ (C) Saboteur ujasniy!
   version changed to 2.10  
21 Version 2.15
   Created stable version
   Added account command
   Added message in blind spell
   added saboteur.c (27.09.99)
22 Version 2.20
   Added offline addqp
         offline removeqp
         offline load
   Fixed arenarecall crashing bug
   Fixed Thrott container (pickproof)
23 Fixed memorize
   some rewrittens
   added PK_RANGE function in io.c
   fixed accounter
   version 2.21

=-~=-~=-~=-~=-~= НОВАЯ ЭРА - пишем по версиям. Saboteur : =-~=-~=-~=-~=-~=-~=

2.22 - current version
     Skill cost changed: 
      if current class have this skill, remorted players get cost+1 trains
      if do not have - cost*remorts
     Fixed bugs..
2.23 Fixed bug with unknown liquids
     Fixed clan.area fix exits messages.
     Fixed autoonline at last.
     Fixed rename
     Fixed offline
2.24 Fixed detect invis and detect hidden for permanents (druids)
     Fixed offline addqp remove qp
     Added show option for offline
2.25 Sprites now has mana regeneration specials
     Sprites are now resist to charm, vuln to Light. Removed vuln to acid.
     Removed name check on mobs, when creating name.
     Recite spell now has 2 or 3 rounds waitstate, depends on level difference.
2.26 Fixed gain_skills
     Zombies & Skeletons are free now for eating, drinking.
     newbies ( <5 level) condition/2
     rewritten quest generation program
     add area flag noquest
     removes frenzi lookup, when using aid
     hunger chars will not die now, hp will freeze at 1.
2.27 may be fixed saving..
     Fixed sprite mana regeneratin for PC :)
2.28 Crush bug Fixed
     Hunger Damage fixed
     Fixed reform in army (added noreform area flag)
     Fixed auction in army
     Added birthday greetins card in weather update.
2.29 Fixed tail
     Birthday card turned off
     Fixed clantalk and auction channels on/off
2.30 Clanwork command complete. Clans data are ready to creating,
     removing, editing and using.
     Fixed criminal in fight and magic.
     Fixed adrenalin.
2.31 Steal shopkeepers allowed
     Added memorizing stealers
     Reoptimizing db.c
     Fixed some bugs
     Added fix_keepers function - all shopkeepers are now 99-109 level...
2.32 Fixed Nopass
     Fixed stealing (max stealing level*3 gold, level*50silver by one time.     
     Small upgrading clanwork.
     Added seen command (Cortney's seen removed)
2.33 RACE_SPECIALS Rewritten
      added spec vairable to pc_race_table.
      replaced checks for flying, crushing, noeating... races
      all race_specials now can added by editin pc_race_table.
     Added NoQuest and NoSteal act flags. Nosteal  for clan shopkeepers
      be cause they are have SO MUCH moneys!
     Fixed put in closed container.
     Added leader command.
      added remove option to leader command to remove from clan in offline.
     Leaders can see clan rank offline by seen.
2.34 Fixed nosteal flag.
     Fixed death counters. 100 qp to Magica
2.35 Fixed frenzi under Aid
     Fixed rescue nosafe chars. (150 qp Creator)
     Fixed show command (100 qp Otark)
     Reboot upgraded
2.36 Fixed flee for mobs
     Mobs can't fly and swim without correct spells and specials.
     Lizards and Snake are can swim now.
     added check for water (damage if you hav'nt boat/fly or swim spell)
     zritel do not work at arena.
     added config shortflag
     Sanctuary now has black and white auras for evil and good chars.
2.37 Fixed crash bug with chars in pet shop. (150 qp to Margarita)
     Test command shows pet shop - nado postavit safe flags na komnatah
     s jivotnimi.
2.38 Fixed seen (50 qp Mudmonster)
     Fixed damaging shopkeepers inventory (100 qp Ultra)
     Fixed buing NULL pet.
2.39 Account do not clear by remort
     Added clan independent option to clanwork
     Created clans.dat
2.40 Fixed show command (50 qp to Matrix)
     Seen now do not show immortals logins to mortal (50 qp Antarex)
     Spam (15 commands repeating) rewarded by waitstate 3
     Crush - waitstate victim 1-4, char - 3.
2.41 Fixed sanctuary
     Added blacksmith act
     added do_blacksmith
     added item_enchant type (olc, tables)
     added regeneration spell (elf and druids has good regeneration)
2.42 Fixed kucha bugov s blacksmith
     Now Blacksmith WORKS! :) and has 11 affects. (in olc change v0 for affects)
     Fixed follow
2.43 Added raise option to Leader command.
     Fixing warnings.
     Remove clan rooms (useless)
     Added enchant item statistic to ostat
2.44 Fixed water sectors !
     Fixed warnings.
     Blacksmith command gives to DWARF! and removed from mobs.
     test command shows affects.
     regeneration spell added to 'regeneration' group for clerics by 4,5,6 CP.
      for cle war mag.
2.45 Regeneration updating Fixed
     Cosmetic changes
2.46 last fixing.
2.47 affects now shows in stat aff command, and removes from qstat and mstat
     cosmetic changes in act_wiz.
     some fixes in save.c ( %s host fixed)
     fixed wiznet
     added few flags
     Mortals can't now steal from Immortal.
     added male female option to who command
2.48 Added AHELP command - advance help.
     Fixed quest info with obj_quests.
2.49 removed 'bug', 'typo' and 'story' command - useless
     Added spamprotect flag to commandtable and rewritten spam protect system.
       IT IS COOL NOW :)
     Added {+ and {- symbols (Flash on and off)
     Added getexp and autorecall to config command.
     Drow now learn advanced casting of spell regeneration.
     max_parameters now 30 points general limit, and personal limit for races.
     May be fixed criminal in some commands.
     May be fully fixed attacking in safe rooms.
2.50 Fixed quest generation .. nu i bug bil :( (100 qp to Margarita)
     Fixed noloot (inversed) 50 qp to Dinger
     Fixed bug with floating disk
     Added Invader feature
     Added passwd to offline show
2.51 Fixed MANY bugs with high.stat, invader feature, cast Offensive spells.
2.52 Ogres Spec_Regen now right.
     Added set pulse command to immortal
      - this option changes interval of tick. default - 4.
     added SPEC_ENERGY for Vampires and Drow - advanced energy drain.
2.53 Fixed criminal (inversed)
     Fixed clanrecal and recal under adrenalin.
     Fixed getexp.
     Getexp is ON on default
     Cosmetic fixes in act_comm.c. Added noarmy flag to command table.
2.54 Fixed load mob crashing bug.
     Cosmetic fixes.
     Added suicide command.
     Fixed death_pc_total counter.
     Fixed bug with getexp off and deaths.
     Removed violate - useless.
     Optimizing act_obj.c and comm.c
     Removed shutdown - use reboot down/reboot shutdown option.
     Added spec_dodge to hobbits.
     Removed adv_energy from drow.
     Added dex check to steal.
     flash symbols autoremoving from titles.
     May be fixed crash bug with sacrifice morphed criminal chars
2.55 Fixed crash bug with sacrifice morphed criminals.
     Added noadr - no adrenalin flag to command table
     recal, clanrecal, arenarecal and suicide are not works under adrenalin
     function send_to_char renamed to stc. define removed.
     Gives Fireproof artefact (21 level) after army.     
     Gives ADDVAMPIRIC artefact (101 level) to remorted vampires.
2.56 Db.c: added K-symbol to load areas - compatible with areaed.
     added greetins.
2.57 Fixed energy drain - can gain hp maximum 110% of max_hit.
     set race now will set race_affects immediatly.
     added criminal to prompt.
     Some fixes and optimizing.
     Fixed army artefact
     Fixed rename
     added host and time login in oscore
2.58 fixed harm (2 round waitstate)
     added new clan bonebrakers
     removes curse from dispell
2.59 added nodelete flag
2.60 Resurrect updated for levels. magic numbers 30, 60, 90.
     add previous host information at reconnect.
     added very simple Password crypting.
     chars with crypted password now has version 7.
     renamed BoneBrakers to BoneBreakers.
     added small procedure for move chars from bonebrakers to bonebreakers.
     noreform noquest flags set to limbo.
2.61 Log detalized
     GET_EXP is on default for new chars
     removed snaker's Race
     removed bonebrakers. (bonebreakers now)
     removed moving bonebra-bonebre proc.
     offline show now shows password again (was encoded)
     Fixed povestka.50 qp to Blackheart
2.62 removed galimiy pwd crypting
2.63 added autologout interval.
     added it option to who (wizh sex none)
     fixed grammatik in do_nodelete
     added config autologout. New chars autoset to 5
     FIXED bug with customizing char after remort
     rewritten summoner.c
     Fixed adding trains from released CP after remort.
2.64 Autologout now saving.
     now cant go libmo in fighting.
     ogres regeneration now *3 not *5
2.65 Added 'set weight 0' artefact
     fixed seen (do not mob checks now)
     fixed stopfol under Charm spell
     fixed quit under SLEEP spell
     Mass heal now heals only group if group exist. else heal all in room.
     Save_one_char fixed (I hope)
     added global command (now only global save)
     wizlock, newlock, birthday moved to global_cfg flag, and now saved to file
     Some changes in check_parry and check_dodge.
     added nozrit flag to command table - to prevent bugs on arena
     Deaf inversed (Gizmo 50 qp)
2.66 Rewritten clan data. Now CLAN_DATA using only. all code rewritten
     Some changes in do_who
     guild rewritten
     petition command enchanced - now list and cancel options enabled
     fixed bugs with race max_stats and max_train_stats
     changes some stats in race table
     Sanctuary now do not updatings on mobiles
     Fixed may be bug in load_clans
     added diplomacy command and clan variable. gluchnaya strashno.
     Written add_word and remove_word functions
     Fixed diplomacy. Its works now!
     Diplomacy now: alli - are safe,  war - do not checks criminal.
     kill command now avaible for killing PC.
     little optimizing in commm.c (~1.5 kb free)
     added help for diplomacy in code.
2.67 added config getall option (get all corpse after death)
     fixed who command 
     added get_pchar_world function (get only character,not mob)
     some functions with player now works better and faster
     and fixed seen/etc command
     gate now works on alli clan members
     added clan->flag int64 variable
     independent renamed to loner and moved to clan->flag
     rewritten clan.c
     added status and toggle options to leader command.
     added ALLI channel for allies clans.
2.68 changed add_hp formula (size check added)
     add_mana and add_move changed
     giants are now HUGE
     zombie now is resist to weapon
     resist now 33
     resist now works on breath,  but vulnerability - not.
     vampires now has -3 to all stats from 6 to 19 hours.
     chain now is DAM_LIGHTNING (was DAM_NONE)
     stone_skin AC calculation is changed
2.69 fixed gate spell
     rewritten petition command - fixing bugs
     little changes in save chars
     read messages (note/idea..) by number will not affect on unread messages
     fixed music boxes
     clan members now dying in their clandeath rooms
     fixed vampires stats time
2.70 petition fixed
     nogetall toggled. default - off
     fixed self argument
     fixed ogres
     fixed ALL STATS
     fixed get_curr_stat
     added clanrank to whois
     stone skin now 310 max
     scroll of recal do not work under adrenalin
     fixed dam_harm
     fixed open/close
     alli now is aclan. and fixed
     global fixed
     wizlock fixed
     diplomacy now checks CLAN_LONER flag, not loner clan
     fixed save
2.71 Fixed put all <container>
     Fixed getall flag
     config is colored
     fixed 2 bugs with fpReserve
     clanwork now 105 level
     crush skill fixed
     suicide now decrease exp
     added Sunray spell (C) Sart
     combat spells are modified (C) Sart
2.72 recall allowed under adrenalin
     aid now self spell
     fixed aid add_stats bug
     fixed switch command
     fixed petition and leader commands
     fixed quest cancel in quit
     SPELLS GREATLY IMPROVED :) now casts on equ, inv, room
     recall in battle -25 exp. chances 50%+daze*2
     nouncurse and reminvis artefacts given to Unicorn
     wear function was CHANGED (C) ADRON
     all items moved to inventory. (C) Saboteur
     fixed online command (bug with multiple args)
     looting fixed
     added dual weapon for thieves and warriors
     huge can wield 2 two-handed weapons, large - two-handed and simple
     thieves can use only dagger as second weapon
2.73 reduce of skills from daze state lowered
     chance of bash changed
     bug with two necks in new char version fixed 
     fixed problem with the scroll of identify
     check_shield_block fixed
     remove_two_hand_flag artefact added to Unicorn
     char now has GREAT PROBLEMS when removing a weapon in battle
     remort fixed - saving added
2.74 magic missile fixed - produce level/30+1 missiles
     earthquake changed - damage increased & daze_state on 1 battle round added
     dependency on dexterity added to check_dodge... hobbits need fix? :)
     fixed saving char in remort
     added nogsocial feature
     resist, immune, vulnerability now will save.
     added flags variable to skill table
     added C_NODUAL flag.
     fixed disarm bug
     global command rewritten
     added global deny function
     wizlock and newlock moved to global
     call_lightning and sunray fixed - added is_safe check
     zap fixed - can use only wand in left hand
     Hi. It's me. I'm to foolly to understand what I'm doing :(((
2.75 Fixed vampire race
     Some changes in races stats
     added humming artefact
     humming artefact given to unicorn 150 qp
     added comsys comm flag
2.76 humming artefact is 1500 qp now
     fixed warning with check_skill
     fixed heal uncurse
     fixed petition_list crashing bug
2.77 Fixed asave changed bug
     Removed ascii.c and do_map commands - useless. (Adron zabil na eto delo :)
     removed automap setting.
     fixed resurrect HP overflow
     plague and poison damage improved
     fixed space in who for nonclan chars
     added room command
     added message with calm fail
2.78 Fixed noclan in do_who
     Fixed alias (Invader)
     input size is 4096 now (Invader)
     Many fixes from Adron, wear <item> [left] syntax added
     tell command now uses get_char_room, get_pchar_world
     Magic missile fixed
     flamestrike improved
     added new skill for clerics - pray 
     fixed Invader's input
     and pray fixed
     Fixed stat mob fixed
2.79 improved armor if target=char itself
     only missile weapons are usable for throwing but there's no need in free hand now
     chars can't be attacked by the command kill
     fail in praying now gives weaken or curse or sleep
     cost of pray increased
2.80 Fixed saves
     added spec command
     removed fly command
     invis and fly options added to spec command
     wearing two-handed weapons fixed.
     missile weapon fixed can throw from inventory.
     vnum upgraded
     affected_by now 64bit
     added frenzy affect
     cancellation spell returned to Healer
     social peck translated (C) Sab :)
     SPAM flag added to some command 
     waitstate for spam doubled
     popitka to fix ban
     frenzy fixed
     get_char optimized in tell command
     clantalk and immtalk now remembered in lostlink
2.81 fixed tell
     fixed clantalk and immtalk
     fixed heal cancellation
2.82 save_char_obj_backup removed - useless. use save_one_char with SAVE_BACKUP
     rem_two_handed now 2500 qp
     feature for closed doors in exits, autoexits and prompt exits.
     fixed clantalk with invisible chars
2.83 fixed criminal with dirt, bash, trip, crush, tail
     small resist/vulnerable change
     for 15 death from mobs or 3 deaths from chars -10 maxhp (minimum 20+5*level)
     added auto option to online command
     pray fixed 2 times
     fixed spec_xxx
     check_dodge improved
     Psionic added and given to half-gryphon
     Psionic addins 5% level to casting charm and sleep
     Psionic gives more chance to cast under daze_state
     drinking BLOOD for vampires added
     arenarecall now from 1 level allowed
     murder is allowed from 1 level
     affected_by shows by affect command
new  killing in army do not affect -hp
     quest complete and cancel will now save chars.
     pray removed from gain for non-clerics classes
2.84 added "tipsy" for some commands
     added clan variable to area_data
     added check for anchors in clans - they will be moved to Hassan
2.85 bounty in army fixed
     remove curse on mobs allowed but not on their items
     taking items to army due to auction fixed
     quiting in clans fixed
     fixed quest time
2.86 fixed wiznet crashing bug (added smash_dollar function)
     fixed crash bugh with long argument
     fixed deathcounter
     int64 applied
     fixed deathcounter
2.87 observation spell added to detection group
     quest convert allowed on 101 level
     quest buy item not allowed if level<22 and remort=0
     fixed setting proper value to field remort when making set ... class...
2.89 int64 implemented by Jasana and Dinger
     silver and warnings fixed by Saboteur :)
     may be fixed steal..
     added 'all' argument to do_affects
     autorecall fixed
     fixed spec fly and spec invis
     fixed do_score
2.90 in advance_level mana upper border is now 60
     quit is not permitted under charm
     aclan is not permitted in army
     fixed vnum
     fixed asave
     buy now not spam protected command
     fixed do_who
     some syntax fixed
     added tipsy to offline
     quest time changed
     added do_printf by Saboteur
2.91 fixed too large damage in crush and tail
     exp now depends on how much damage you did by yourself
     identify crush fixed
2.92 fixed int64 area saving and many bugs
     added who pk command
     fixed some bugs
     optimizing some code.
     changed note list command
     added some info to leader show
     some changes in printf (restrict, formatting..)
     socials check rewritten
2.93 fixed whois (lastsoul 50 qp)
     fixed group (rumata 50 qp)
     fixed who pk (plague 50 qp)
     fixed healer (plague 50 qp)
     may be fixed aggr_update crash bug
     fixed noloot (25 qp Plague 25qp Diablo)
     fixed do_who (format of classes)
2.94 added special symbol `
     added {- to convert symbol ~
     added {` to convert symbol `
     fixed aggr pet check (Rumata 200 qp)
     fixed deathcounter (Weltmeister 250qp)
     fixed dual weapon
     fixed list pets (25 Rumata 25 Weltmeister)
     create_money fixed
2.95 fixed sneak phraze (25 qp to Plague)
     rewritten global command and removed const from cmd_table.
     now global deny <command> can be saved
     some code optimization with register prefix
     mobs can't now take quest items.
     Inserted bug, that fixes dual weapon_sn skill :)
     Earthquake and GasBrearth now cant cast in safe (500 qp Helly and Angell)
     Fixed do_score for <15 level char) - 25 qp to Plague
     Fixed areaedit_show (noreform flag now shows)     
     Fixed do_inventory (25 qp to Plague)
     Fixed crash with act_obj
     Fixed deathcounter in army
2.96 Fixed some \n\r symbols
     can_see now don't check in do_send for victim
     Nodrink races now refreshes after 1 tick.
     Account fixed
     Added NOREFORM flag to rooms flags
     Fixed autoquit
     may be fixed crash with HASH, but do not fixed HUSH :(
     12% adding parry chance with dual wield
     added fireshield spell (bash, second bs)
     no summon into rooms with no_mob or solitary flag
     pet will not gate with you to no_mob room
     no quit in room_arena
     no way to use potions & pills in room_arena
     fixed dampool and exp in groups
     fixed double message with heal blindness
     fixed reboot (at last :)
     1/3 damage made by charmed mob added to it's master dampool
     showkills now always shows to char personally
     Tipsy now with waitstate
     added wizlock flag to areas
     fireshield enabled. mages 7 level(cost 3), clerics 10 level(6). (mana 100)
     fixed dual backstab bug
     do_quit advanced
     fixed quest convert (10:50 - 10:500)
     fixed gain cost
     fireshield added vuln to cold, light, fire. 1/2 damage in 20%
     gain cost changed
     reboot now fixed at ALL :E
     ch->level and artefact->level must be equal or higher for blacksmith
2.97 removed double autoassist in autolist (50 qp to Diablo)
     fixed auction - items now gets from inventory only (100qp to Lion)
     some corrects in qstat, and other
     added area_flag AREA_LAW (aggres not attack)
2.98 some corrects in score, whois.
     removed imotd (useless)
     fixed version number
     quest convert 10 prac = 150 qp
     fixed quest in safe rooms
     fixed autoassist with questmobs
2.99 added ahelp, showskill in creating char
     fireshield now has 1/4 damage, 15% chance
     added NOORDER and NOALL flags to command table
     command table improved
     added yell command to CHANNEL
     guards now do not shout in safe rooms.
     great scalpel given to act_adept
     fixed Invader's who
2.99a
     atoi64 added 
     added wait_state to do_quaff :E
     some fixes with quest
     cfg_newyear added (c) Jasana :))))))))
     credits in areas changed
     flag IMM_ALL added 
     sifilis added but not finished
     fixed light (may be)
     try to fix crash bug with input buffer overflow
     do_desc changed (with argument without prefix - shows desc, not change it)
     imm_all added to table of imm_flags - skleroz :(
     some fixes in newyear
3.00 blacksmith now uses get_obj_here
     condoms limit enabled :)
     sifilis fixed
     nodelete now full-typed command
     optimized sunray, earthquake, call_lightning spells
     11% enhanced damage with axe added to dwarves :PPP
     fixed missile weapon level
     fixed noloot bug
   a fixed hash?
     fixed portal and nexus (warp in left hand) - 50 qp to Gendalf
     fixed blacksmith (parsing,  get_obj inv, then room.)
3.01 set race fixed
     damage from harm with IMM_HARM fixed
     blacksmith fixed - if(objt=NULL) :E
     fixed output in do_ear
3.02 act_comm rewritten (about 7 kb free)
     some optimizing in act_info
     now only Midgaard healer kills mobiles
     char and his corpse in army now in same rooms
     reply to army char fixed
     earthquake maybe fixed
3.03 some optimizing in act_wiz
     rewritten reboot,  purge..
     rewritten ban.c
     ban/allow now works!
     quest statistik now decrease only when quest cancel is proceed.
     fixed recalls with moves<2
     fixed questmob deaths with from poison
3.04 added check to quest_generate for mobs in not their area
     fixed ahelp (formatting)
     fixed whois (no clan members)
3.05 fixed quest phraze
     added quest room to quest info
     account percentage fixed
     added referi command (referi show - show affects on char)
     added referi act flag for mobs
     flag_type64 changed to int64
     some fixes
3.06 added second dual attack
     spec sneak added (vampires, ethereal)
     visible command now remove invis, sneak, hide affects for all
     added sneak hide restore commands to special
     special hide added to sprites
     referi fixed
     fixed dual weapon, second dual attack
     artefact level now added to blacksmith_level/5 for item level
     quest convert allowed from 90 level
3.07 fixed dual weapon work
     fixed dual weapon sn for improve checks
     fixed reboot !!!!!
     rewritten quest cancel (optimizing)
     itempurge,  mobpurge removed (useless)
     bounty optimized
     fixed socials with mobs
     many small fixes and optimizes
     random number generator turned to another version...
3.07a fixed aclan crash bug
     rewritten startup
     added reboot_counter - 10 reboots within 5 ticks SHUTDOWNS the mud!
     removed few useless functions in startup and code
     removed printf in fix_exits
     added load area name to log
3.08 interp.c updated
     Nostalgia added to aggrs
     clanrecall from battle now takes 30 exp
     zombies recall fixed
     Executioners do not check immortals :E
     Zombies now joins clan of their master automatically
     Percent for banking changed
     dual improve fixed
     no qstat on mobs
3.09 train hp and mana now restricted to 3 by every level.
     (char trained statistic added to qstat)
     get_pchar_world rewritten
     get_online_char added
     contest help added to purge
3.10 notes now have field host visible only to immortals
     clanenchants added
     wiznet options trust fixed
     hide now allowed only in POS_STANDING and removed in battle
     function can_attack added
     now char can not attack under calm
     12 attacks after rescue maybe fixed
     Начата новая эра записи русскими буквами. А кто не будет - то араб.
     Переделаны бочки - они теперь не бездонные.
     Переделана liq_table - все жидкости по-русски.
     Добавление хп за кровь временно отключено.
     При смерти и суициде хиты мана устанавливаются в 1, мувесы - в 2.
     Вороватое убийство разрешено.
     Теперь длина титула не определяется "цветовыми" символами.
     В ptc и do_printf добавлен учет "цветовых" символов при форматировании
     длины строк.
3.10aДобавлено обнуление счетчиков хитов и маны в реморт.
     После смерти у чара остается четверть маны, а не 1.
     Пофиксено нападение под калмом-блиндом.
     В load_objects(db.c) добавлено автоматическое снятие флага кланэнчант.
     Пофиксено всякое разное с кланэнчантом.
3.11 добавлен апгрейд waybottle в save.c
     пофиксен глюк с реконнектом
     добавлена опция clan в aedit
     fixed reboot_counter overlock
     added new note group - bugreport
     fixed some bugs with ip_address - unknown now will show ip address
     added Penalty_data - пока еще ничего не делает.
     пара файлов избавлена от лишних enter
     перекрашены ноты :)
3.12 Саб, ты араб (С) Мы :)))))
     Изменен raw_kill - теперь все вещи кроме floating и rot_death остаются
     у чара. Труп с деньгами остается на месте смерти.
     Изменены уровни появления заклинаний из групп harmful и healing.
     Harm wait_state - один боевой раунд.
     quaff wait_state - один боевой раунд.
     Теперь можно поднимать зомби из трупов игроков.
3.13 printf parsing вынесен в отдельный файл printf.c
     rewritten string.c -
     переведен, добавлены функции string_remove и string_show
     теперь можно использовать ~h вместо .h в string edit_mode
     train hp fixed (не сейвился)
     ЗЫ сами вы Арабы :E
beta fixed log_string (abrakadabra)
     some fixes in output
     добавлена функция log_printf()
3.15 added some check_parse_name for new and old chars names
     fixed bug with load_exits
     теперь квестовые мобы не умирают от поисона
     переделана функция cancel_quest
     сильно переделан quest.c
     теперь добавить новые вещи в список - 1 минута
     пофиксен вывод echo_on_string и echo_off_string !!!
     исправлен склероз с уровнями хилинга у воинов 
     добавлен новый спелл только для клериков - spirit lash
     пофиксен do_fill в плане вэйботтлов и нескольких фонтанов в комнате
3.16 сохранение прочитанных бугрепортов fixed
     в alist добавлен клан
     fixed quest time after reboot
     fixed comm.c warnings
     added do_act function with spam protection integrated system
     use 'doact' alias for enable this feature
     added spam protection personal system v 1.0 (worked)
     added command 'spam' to configure antispam personal system :)
     spam feature integrated to disarm and bash commands
     re: english - rulez. Sab.
3.17 quest time by Guru (200 qp)
     fixed quest info
     fixed quest cancel with no quest
     log may be fixed
     fixed drink containers
     fixed remort (stats, ...)
     fixed do_flee (stop_fighting)
3.18 Переделаны опции управления каналами (табличка в act_comm.c)
     команды deaf,quiet, show переместились в channel
3.19 добавлен pcdata->cfg
     уменьшение счетчика трайнов при отнимании HP
     NODELETE перемещен в cfg
     fixed quest complete при квесте на итем.
3.20 fixed FLEE (do_stop_fighting function added)
     IS_CFG macros added
     ZRITEL,NOSNOOP,SHOWKILLS,AUTODAM,AUTOEXIT,AUTOTITLE,SHORTFLAG,GETEXP,
     GETALL,AUTORECAL,AUTOSPLIT moved to pcdata->cfg
     few optimizes in act_obj.c (~10 kb)
     spam otherfight - не выводит чужие удары и chainlight
     spam miss       - не выводит твои промахи
     spam skillmiss  - не выводит промах твоих скиллов
     string.c - добавлена возможность вставить строку в произвольном месте.
     noloot command removed - useless
     backup improved:
       backup show <name> - показать информацию про бэкап персонажа
       backup restore <name> - восстановить персонажа из архива
     run command added,  but did not work yet - only argument parsing ready
     run command now work, added do_move_char function, run function.
     fixed bug with crash on reboot (неправильно отменялся квест)
     disarm left added
     fixed gold&silver counts при подбирании.    // А вы Арабы :)
     shortflags now Very short
     whois criminal теперь выводится только при наличии флага
     removed flag_type
     added config noimms option - не слышать смертных по имм каналу.
3.20bfixed run and flags
3.21 added CLANBANK command
     rewritten clan options
     rewritten save.c для исправления бага с потерянными итемами.
     пофиксены бездонные фляги - бездонный контейнер value[0] = 999
     restored drinkind blood for vampires - 9hp for 1 drink
     Теперь короткие флаги выводятся и для предметов
     delete fixed
     short flags переделаны
     clanbank now works
     пофиксен run мимо аггров
3.22 Wiznet and log fully rewritten
     Spam otherfight now works - уже один раз кодил,  пропал наверное :(
     auction stop теперь работает
     добавлена проверка на аукционщика, который вышел раком.
3.23 spam_miss теперь влияет на уворачивание, блокирование и парирование.
     fixed disarm
     fixed inversed spam config
     fixed reboot now :E
     fixed run (trying to unlock all doors, even if you under pass door)
     пофиксен дополнительный удар кулаком при наличии скилла дуал и оружии-двуручнике.
     убрано повышение второй-третьей аттак если в руках не оружие.
     Сифилис снимается при смерти.
     Мобы сифилисом не заражаются.
     Написан новый is_set. Надеюсь теперь варнингов не будет. 
3.24 fixed dodge/parry/shieldblock, if learned=0 (100 qp to Kleopatra)
     trying to fix auction
     some optimizes in act_wiz
     added spam_weapaf option (cold, flaming, shocking effects messaged)
     fixed account (округление в большую сторону теперь работает верно)
     fixed limbo bug
     5% chance to cure sifilis with cure disease
     Sab
3.25 deny suicide under charm
     yell now allowed in army
     fixed stop_idling morphed chars
     race_spec ahelp improved
     убито несколько варнингов
     чуть-чуть переделан сифилис
     юзайте презики :)))
     ностальгия поправлена
     добавлена проверка на null char в save_one_char
     %Q - quest points in prompt
     пофиксено попадание мобовских трупов к хилеру
     rewritten quest.c
     добавлен новый тип квестов - отнеси это кому-то.(1/20 chance)
     пофиксен run через закрытые двери
     маленький фикс raw_kill
3.26 fixed leader command,  some fixes in do_petition
     добавлен бег до упора в команду run (NWESDU)
     randomize quests
     fixed showkills
     fixed qstat
3.27 DWARVES GUILD added
     blacksmith add, remove, unequip options added
     some fixes with SET_BIT, IS_SET, REMOVE_BIT     
3.28 some fixes with crash in extract and quit.
     some grammatics in guild for Dwarves guild
     в blacksmith теперь проверяется принадлежность к Гильдии Гномов
     добавлен stat adv <char>
     почищен qstat <char>
3.29 kazad channel added (Dwarves Guild)
     room flags blacksmith and dwarves_guild added
     gaccount (guildaccount) command added
     в account и gaccount можно класть на счет брюлики и кристалы.
     пофиксен краш в leader (I hope)
     добавлена опция note edit - писать или редактировать текущее сообщение
      в строковом редакторе (как room desc в OLC)
     note to <race> теперь работает
     (fixed note edit, accounts)
     Sab
3.29a fixed unequip levels and phrazes
      fixed auction
3.30 optimizing account and gaccount. sell_gem function added.
     added penalty_data, remove_penalty, update_penalty functions
3.31 fixed waybottles
     rewritten itemlist
     added moblist command
     feature to save ch->on object added
     added checking for NULL in get_obj_desc
     optimizings in mob_progs
3.32 blacksmith now using formula:
         item->level должен быть меньше или равен
            UMIN(dwarf->level ,artefact->level + dwarf->level/6))
         Для гнома, не состоящего в Гильдии его левел везде делится на 2.
     fixed "look in" for waybottles
     changed quests randomize
     added penalty add command
        (beta version - only nochannels for some ticks works)
3.33 medit_show changed in olc
     added ACT_STAY_PLAIN to mob acts - moves only within ony sector type
     added log for adresses - collect info for resolving adresses
3.34 fixes with race :(
     log adresses changed
     added blacksmith help
     ally channel inversed
     added dwarves_guild door feature
     fixes socket security
     fixed offline security
     stranniy bug with diplomacy krivo fixed
     added vote command, but do not work succes yet.
3.35 vote command fully implemented. vote help to see help
     offer command beta version implemented (for assasin guild)
     online command now supports clan names
     немного изменена формула ковки -так как мы давно договаривались
     ковать теперь можно только в кузне (как и анэквипать)
     нападать на петов можно, только если можно напасть на его хозяина
     в olc_save добавлен "пропавший" энтер
     Sab
3.36 added assasin guild
     AssasingGuildGold added, accounter for guild added
     offer command enchanted, offers saves in config.dat
     sh_int removed from code
     offer command implemented. offer help to see help
     ASSASIN_ACCOUNTER flag added
     Assasin guild implemented
     ASSASIN_MASTER flag added
     fixed recycled for vote, vote_chars, offers
     FIXED save_config, save_clans, save_vote !!!
     added note_send function
     clanbank improved : clanbank now can get diamonds and crystals
     clan members now can get gold from clanbank
     some messages added to clanbank
3.37 fixed gaccount for assasin guild
     mana info added to showprac
     fixed olc flag int64 settings
     material table implemented, materials saved from table
     material_lookup function added
     guild command rewritten for using by mortals
     guild info now shows in whois for self and elders
     added can_see check to bounty list
     olc_save optimized about 5kb
     online fixed
3.37b fixed recycling offers
     fixed whois (blin, v 2 nochi nado vse taki spat' :/)
3.38 offer crash bug fixed
     vote loading fixed
     vote summary statistic now shows to mortals
     added vulnerability and resist checking by material table to check_immune
     material now shows in lore
     find_location fixed (now alias like 'goto xx' will be work)
     added no_magic flag for rooms
     added hard flag for rooms
     move_char removed (useless) (use do_move_char)
     config runshow worked on carried morphed chars
     heal cancel do not check anticancellation
     do_heal moved to magic.c, healer.c removed
     do_flag moved to saboteur.c, flags.c removed
     traveller code moved to saboteur.c, summoner.c removed
     lookup code moved to db2.c, lookup.c and lookup.h removed
     fixed position in do_stop_fighting
     small bug in do_disarm fixed
     whois fixed twice (guilds info)
3.39 act_enter.c removed, code moved to act_move.c     
     alias.c removed, code moved to interp.c
     scan.c removed - code moved to act_move.c
     ban.c removed - code moved to db2.c
     rewritten clan enchant data (pootrivau ruki, kto tak structuri sdelal)
     fixed diplomacy:
       rewritten string works - add_word and remove word removed
       added add_clan, remove_clan
     fixed save_clans (double recalmsg2, optimizings)
     is_safe - added npc check...
     advance rewritten for self using
     added gspeaks option to wiznet (speaks now - tells, clans, emotes...
                                     gspeaks - chat, gossip, gsocial...)
3.39a immortals dwarves now do not get waitstate with unequip
     added zlaya ficha for Lion Utka Belka Doomer
     fixed mptransfer (GREAT BUG)
     pc corpse left in room where char dead.
3.40 added clan private voting. second and leader can add and remove vote
     fixing bug with crash in extract_char
     raffect structure added
     added raffects: RAFF_BLIND
                     RAFF_NOWHERE
                     RAFF_HIDE
     spell black cloud added to mages and clerics
     max_alias now 15
     found crashing bug with chainlight, but not fixed yet :(
     added feature to add black cloud spell, if highmagic group known
     black cloud now 1-3 ticks length
     protection evil can cast only good chars
     protection good can cast only evil chars
3.41 can_see fixed for obj and chars - infravision allow see chars in black cloud
     black cloud do not allowed in law, safe, arena rooms
     fixed private clans voting
     spec command fixed
     AFF_DEAF added
     call lightning now can deaf chars (1/5 chance)
3.42 fixed reconnect phraze
     fixed infrared
     fixed death log
     HIDE flag now removed when follow someone
     mprog_delay and start_pos for character now used by global spells
     can_see_room check added to scan command
     added 'distortion spell' (roomaffect, 2 casters)
     'shock hit' added to thieves
     added check for ORDEN of LIGHT for:
          wear vampiric weapon
          casting poison, energy, plague, demonfire
          using steal , backstab on peoples
          summon of aggressive mobs
          alignment always 1000
     added obj name to check_shield_block
     fixed some bugs in handler.c (buffer clearing, saving chars)
     fixed dexterity check in dodge (250 qp to Reistlyn)
     some changes in hit chance
     some fixes in stealer and killers memorizing
     Saboteur
3.43 shock hit - 20 level
     пофиксено окончание в фразе (do_shock_hit).
     пофиксена обработка RAFFECTS
3.44 пофиксен додж - коэффициент и порядок
     введена зависимость от инта-виздома для кастеров
3.45 dodge,parry,check_shield_block changed. Saboteur
     one_hit thac_00. Saboteur
     в ray of truth,bless,pray,damage добавлены плюшки для Ордена Света. Jasana
     местами возможно пофиксено попадание в лимбо. Jasana
3.46 fixed material saveing, lookup.
     pc_race_table removed, race_table improved
     added clanword remove <num> - remove affects of clanenchant
     fixed shield block message
     material_table extended and fixed.
     Saboteur
3.47 may be fixed crash bug with add highmagic.
     may be fixed crash with extract char
     Sab
     в конце битвы слетают не только вещи, не подходящие по алигну,
      но и чужие кланэнчанты.
     окончательно доделана ностальгия :)
     после кланэнчанта шмотка стоит чуть дороже - чтоб жаба не задавила
     исправлен глюк с аукционом перед армией
     пара глючных строчек ("заперещено", панимаеш :) ) и варнингов
     Jasana.
3.48 wname race feature added
     hard variable added to material table
     mobs now wear weapons after disarm
     fixed GREAT CONFLICT for commit changes
     Saboteur
     Пофиксен мой ОЧЕНЬ ЖЕСТОКИЙ недосмотр с кланэнчантами. Хорошо, что
     орденоносцы сразу покаялись :)
     несколько изменений в magic.c касательно орденоносцев
3.49 fixed auction stop
     added some racenames
     locate now can locate items with level ch->level+ch->remort*3
     get_curr_stat for mob stats now given from race_table
     Saboteur
3.50 fixed do_move (no automated opens/unlocks doors, if not run)
     fixed crash in auction_talk
3.51 slots removed from skill table, group added to skill table (not usable)
     magic categories settings begins
     vote show fixed
     fixed bug with cycle
3.50a fixed votes.
     immortals can create vote to clan: syntax vote add <clanname> <text>
     пофиксен pray. пофиксен санк для орденоносцев
     пофиксена длина тайтлов
     пофиксено попадание в лимбо при реконнекте после айдла
     уже существующие тайтлы автоматически обрезаются при лоаде
3.51 fixed load title
     removed gluchniy fix s limbo, vozvrashen stariy bug
3.52 classes history added
     rewritten classes
     fixed remort overflow
     class info added to inform command
     groups command removed - useless
     test command used for reset class history of remorted chars!
     some changes in hp_refreshing for spec_regeneration
     name_table removed, class_ntab created, class name echo optimized
     spec_two_hand works for two hands
     races multiple changed
3.53 fixed removing votes
     Magic categories now works!!!
     changes in skills_table, race_table - added bonus and penalties for magic
     added few bonus for remorted classes
3.53a if questmob is wounded more than 50% - questmobselecting false
     some changes in dodge
     added fraze to remove criminal by death
     fixed acid group
3.54 added check for wrong exp
     some fixes
3.55 categories now rewritten
     FIRE, AIR, EARTH, WATER
     BODY, MIND, SPIRIT, align(LIGHT, DARK)
     PERCEPTION, MAKE, LEARNING
     birthday rewritten for Ayaks
     enchant armor improved for makermastery category
3.56 fixed criminal with death message
     fixed getexp for newbies
     mana_cost function now depended for spells categories
     test allowed for immortals
     vote parsing fixed
3.57 bad spells now don't checks saves with self_casting
     travel to cloudy mountain added
3.58 add_clan, remove_clan removed
     add_word, remove_word written
     added marry command
     merried can gate without clan/level checking
     ptalk channel added for merried
     added check on marry and lover to do_rape (for raper and sifilis)
     merried rapers not safe by PK_RANGE
     added superhero feature with restore all
     added divorce command (beta)
     Saboteur.
3.59 fixed liquid_lookup (not crashes now)
     change sex changed
     fixed marry
     added strength modify to damage. 25 str is average (no changes)
     added category info to ahelp race
     fixed equ and look picture
3.60 fixed get_char_desc
     fixed log kills
     added itemlist for special areas
     divorce command allowed only in MARRY_ROOM
     зависимость damag'а от силы пофиксена и несколько увеличена
     кроме того имхо среднее значение для силы - то же, что и для инта-виза
     добавлена процедура fix_weapons в boot_db - ставит типы оружию с dam_type NONE
     имхо опять же это правильнее чем мучить dam_message для не соответствующих
     идее оружий
     каким либо образом попавшие в армию неармейские вещи пропадают после первой
     же битвы
     пофиксено - теперь расам с бОльшим размером дают больше хитов
     (раньше было инт делить на инт -> плюшка = 0) 
     хиты чара фиксятся - см. save.c, fread_char
3.61 few fixes
     hits fix fixed
     fixed get_trust
     added vislist command
     added class command for mortals
     test with dampool restored
     some warnings fixed
     fixed Crushing bug with offline and leader!
     added fireshield check to crush
     added backstab check to str bonus
     fixed race_wname
     fixed online "auto" crash bug
     fixed visible list work with wizinvisible level
beta fixed suicide
     fixed str_bonus, some changes in act_wiz.. warnings, errors.
     fixed can_attack for blinded chars
     fixed ptalk, and invader's gluks
     fixed ptalk.. may be
     portals now can be in inventory/equipment
3.62 message with failed remove curse on self
     messages in slow and haste spells changed
     added sleeping check to socials
     ogres now can't use two-handed weapon in left hand
     afk message added
     fixed criminal
3.63 added censored channel
     ahelp skill added !
     help variable added to skill_table
     fixed marry bug
     added ahelp skill list <groups>
     fixed abs macros
     hunger and thirst damage increased
     fixed kick
     fixed censored for unix - jj is maximum for 32bit :(
     category_bonus fixed
     added learning check to xp_compute (+10 = +20%)
     fixed bug with classname for NPC
     some willsave flags added
     fixed auction stop
3.64 some changes in act_wiz
     clantalk feature for Toxic added
     clantalk now allowed from 1 level
     fixed some levels in command table
     added some remort macros
     category_bonus rewritten and changed (improved)
     little change in asave
     added remove vampiric artefact
     added little hp_fix function for chars, that have smaller hp than must
3.65 Calm not allowed in safe rooms
     wiznet log level added
     sprecall added for married
     wiznet status rewritten
     marry room <name> <vnum> - to set sprecal room
     dodge_check for hr (Imp)
     AC_damage percentage (Imp)
     Heal Light fixed (Sab)
     added features to vislist 
     fixed bug with parry under blind (Imp)
     Combat spell damage now depends from int-bonus vs. wis-saves (Imp)
     Ensnare spell added (Imp)
     Bash changed (Imp)
     Quest move_reward added (Imp)
     fixed race_wname, classname
     fixed note memory overlock
     fixed snoop
     some memory recycling stuff fixed
     censored fixed (Magic)
3.66 added race_data, save_race, save_race_table, load_race (beta version!!!)
     fixed string edit (inserting string with command .<num>)
     crash with note - on localhost fixed. must test on unix...
     new help price added
     is_offered fixed
     cure sifilis fixed
     Sab
     100% enchant from 102 level
     cast without waitstate for 102 and high levels
     Magic
3.67 RACE_DATA removed nafig.
     race_table is not const now.
     race load, race save, race list and show info now works !!!
     race command allowed from 106 level.
     little bug with marry room fixed
     Fear affect added.
     Fear affect can set within flee command
     file_log function removed
     log_printf - log string to stderr/strout and full.log
     fixed many great bugs with log
     NOW LOGS ARE GOOD WORKING! no more EMPTY or NULL log strings!
     for writing logs handle reserved another stream LOGR_FILE (areas/null.log)
     fixed 2 little bugs fwrite_char, fread_char
     Saboteur
     fixed enchant (Magic)
3.68 Ethereals now can flee thru closed doors
     Drow now has 27 max dexterity like Elves
     Many fixes in damage()
     now material resists and vulnerabilities works again
     immunity check fixed
     Saboteur
3.69 spec fly, sneak, hide removed
     added plr_can_fly - for special players U can set this flag.
     now race_special hide and sneak will check in sneak and hide commands
     add command fly
     add command walk
     add affshort option to configs. (command affect upgraded)
     create newspaper create, save, load functions
     kills now logged to newspaper.
3.70 examine command upgraded.(shows wand/staff ammo, and some another info)
     fixed time for vampires
     ahelp stat optimized
     some fixes in do_gain, added internal help for gain
     fixed movements (sector table)
     added get_skill_bonus(char_data, sn) function
       this function used for bonuses on special place, special time..
     casting levels rewritten, removed few glukodromnih functions
     level of casting spell affected by:
       category_bonus, get_skill_bonus, casting class, STAT_WIS
     detect_magic now can show gate summon and teleport destination room.
      gate and summon 50+level_diff% chance, teleport - 15%
a)   fixed ahelp stat crash
     little optimizes in olc_act.c
b)   fixed vampires time.
     fixed create spring
     fixed drink_containers & food in army
3.71 added protect group
     bug with save classdat fixed
     added flag variable to social_data, fixed load/save socials for new vers.
     ahelp group added
     mass heal now allowed in fighting
     ogres regeneration in fighting now /4
3.72 sanctuary fixed
     cast range changed (200-120)
     command smoke added.
     oedit delete <vnum> added (marks object like deleted)
      object will be removed after reboot. asave required.
     socials are now in new format
     command socials rewritten, load/save fixed... etc.
     fixed check_improve (Belka - lamer :P)
     fixed some affects leaks
3.73 sanctuary now half only weapons attack damage
     magic shield now halfs magic attack damage
     social save fixed
3.74 shield spell now cost 75, and apply 'Магический щит' flag to char
     add check for NOGLOBAL socials in check_gsoc
     add movement feature for SOC_CULTURAL
     add check for NOMOB socials to socials and gsocials
     add check for NOSPAM socials to comm.c (rewritten global anti-spam check)
     add Perception check to backstab and steal
     Greatly rewritten command quenia
     added quenia option to global command
3.75 quenia implemented. parsing with victim, words, combinations created.
       Now works 6 words: bless fireproof power restore badspell goodspell
     quest rewards updated: 1 train (5%)
                            1 quenia word (10%)
3.76 newspaper created (command magazine)
     quenia rewards now 5%
     some small changes in quenia
     blacksmith fixed, now non-dwarves can read help
     little fixes with classes, magazines
     quenia info about word change added
     fixed magazine with char death from mobs.
     magazine now uses page_to_char
     fixed train %n/%n
     set class fixed
 r)  russian commands implemented
3.77 fixed quest complete with quenia.
     log_string year cutted
     fixed crash with printf %s in logs
     Limbo bug fixed. Now chars will save correct.
     fixed group expirience gain 
     added tournament (Imp)
     do_tournament optimized
3.78 Vampire Guild added:
        Dark+3
        Light-6
        Spec vampire (add vampiric flag to weapon for some ticks)
        vampires can't take or wear silver items
     some changes in effects.
     effects.c removed, moved to magic2.c
     guild command fixed and vampire guild is added
     magazine improved. added marry, remort news, removed death.
     implemented range 500 events for magazine
3.79 save chars rewritten for fix xerox bug :(
     created files.url -
       for divide configurations for ntu-mud.kpi.kiev.ua & fdungeon.kiev.ua
     dam_light added
     tournament level changed
3.80   rewritten channels workaround (Thanx to Magica for idea :)
       not release version, some channels will bi rewritten by somebody :)
       now most channels can be saved in lostlink (command talks)
     little crash fix with room_is_dark
     race_wname small fix (Imp)
     removed race Babochka from Magica (c)Magica (zadralo, iz 10 igrokov v online
         7 neponyatnih zverej, eto izvrat i ne est' pravilno, a eshe eto pacavatost')
3.81 removed switch,return commdns and  all code in sources with workaround.
     removed pose command
     rewritten all channels, act_comm.c greatly improved.
     noshouts removed. only noemote, nogsocial, notell, nochannels now.
     all channels moved to character->talk, no more ch->comm
     command talk now release.
     say now saves in disconnect
     deaf and quiet commands returned
     aclan renamed to alli
     renamed fixed (old pfile now will be deleted)
     fixed train hp/mana information
     vbite for vampires - special skill vampire guild added
     material table improved. Materials changes damage
     criminal and adrenalin fixed
     fixed character idle timer
     oset material added
     fixed magazin info
     wizlist updated
3.82
     fixed do_say (mobtriggers)
     fixed reply (in sleep mode)
     ban now allow to ban sites, not adress
     wizlist for both world now works.
     global_cfg IS_LUCKY - detect that is ntu-kpi or lucky.net mud
     Saboteur.
     Random weapons implemented (Imp)
     Memory leak in reset_room() fixed
     Random item generator changed ( upg on high levels )
     quest random generator now works GREATLY! (Saboteur)
     removed flag_skip in wiznet - useless
3.83 fixed heal light
     fixed magazine recycling stuff
     fixed do_look for wizinvisible players
3.84 Fixed bug with quenia bless word
     add config tick option
     fixed all saving chars (Xerox bugs :( )
     fixed room raffect
     fixed do_log
     quenia bad spells in safe
     fixed infrared in dark rooms
     fixed adrenalin with bash miss
3.85 fixed ear messages
     fixing log_string (very long strings)
     add mist command for vampires
     gsocial fixed
     crush change decrease
     flee now with waitstate 1 (1 move cross room)
     Astellar fixed
     fixed newspaper
     global random added (Imp)
     hobbits now have adv_steal (+15%) drow +7% backstab.
     Expecting changes from Magica :) (Imp)
3.86 fixed bugs with overload in editing strings (notes, descs)
     removed .r (replace strings)
     fixed delete and insert strings.
     little changes in security
     aedit builder changed
     vampires mist changed
     howl command for ethereals added
     fixed subtitue alias
     fixed BD frequenc.
     global random save fixed (Magica)
     fixed offline info if trust(ch)<trust(victim), it's work only for imms (Magica)
     removed password and qp from offline show (Magica)
     added offline password|stat|nopost|nochannels (Magica)
     add secure olc flag to offline (Magica)
     add secure olc flag to offline (Magica)
3.87 Saving config changed
     Added localmode check for disable immortal security on localmode
     rechecked class_bonuses
     ahelp class added (plain text :( )
     Saboteur
     alloc_mem and free_mem _greatly_ rewritten
     that may be slower, but more safe 
     memory command updated (Imp)
     crush fixed
     fixed reboot "-1"
     fixed get_full_class
     BODY group improved
       cure light, serious, critical and heal greatly improved.
       regeneration hp depends on caterogy_bonus
     add flag drent - in this room char will pay 1 gold to dwarvesguild account
3.88 added guild.c file for all guild feature
     gaccount, guild, save/load guild data was rewritten
     removed offer for items
3.89 heal spells rate changed
     freeze command changed
     added MAGIC WEB spell
     spell distortion changed
3.90 added hand_damage to race_table - for use by races without weapon, if
      dam_type for mob set to DAM_NONE (0)
     changed number_range number_percent formula (c)Magica
     teleport is not allowed to area battlezone (Imp)
     db.c messages (Bad format in fread_xxx) now shows file offset (Imp)
     fixed guild for mortals
     fixed autoadd magic web
     Some compilers' warnings fixed (Imp)
     another warnings fixed (Sab)
     shield block do not checks on whips
     daze_state improved (worse flee)
     psionic improved (some resist for daze_state)
3.91 add check for ACT_CLERIC on mobiles heals spells
     get_char, mob, mobindes.obj, objindex desc fixed (str_dup)
     race with DKICK can start battle with kick
     Sab
     owhere random crush (Magica)
3.92 reboot now allowed for all immortals and all coders (flag COMM_CODER)
     bugreports can read only coders
     fixed possible crash with order (thanx to Belka)
     do_train rewritten
     get_max_train for hp/mana now 2, 1.5, 1, 1
     fixed ogres two-handed for left hand
     add SPEC_RESBASH - chance/2 for bash, damage/2 for crush (centaurs)
     gain_hit is less randomized
     mana_gain is less randomized
     fixed trains, 1.5 max
3.93 fixed criminal on sleep
     remove assasin_guild flag
     some changes with thac in one_hit
     offline info and show merged
     ogres refixed
3.94 silver for vampire guilds fixed - now they can take silver, but can't wear
     fixed curse for zombies
     willsave flag upgraded
     small refix bug with alias
     convert hp to tra (Magic)
3.95 staff and missile weapons added to weapon_master
     berserk now affected on do_cast (75%)
     one_handed removed from unicorn
     vorpal now has more chance to hit thru parry, shield_block (on 10%) Belka.
3.96 immune to curse fixed
     rewritteb backstab_part in one_hit - moved to do_backstab
     removed dt parameter in multi-hit
     fixed load/save guild data in pfile
     added logging pulse areas,obj,mobiles
     smash_beep function added (no more {* beep spamming in tells and channels)
3.97 fixed mob's hand-to-hand damage
     removed old functions for loading obj, mobs, areas
     added define for restricted mud compiling. (in merc.h define public)
     fixed curse
     again regeneration for ogres
     add check for negative expirience
     berserk fixed
     backstab in fight fixed
3.98 fixed public
     fixed damage, damage messages...
     fixed mobiles damage with and without weapon
3.99 fixed backstab (crash, dam type)
     know align now rewritten to know person
     added get_clan_rank, get_align functions
     berserk, frenzy improved
     berserk lost concentration is about 17%
4.00 autolook option added
     imm ban added
     fixed checks on backstab and missile weapons
     some interpret and subtitue alias fixes
     fixed expirience for group cheating
     add check for silver equipment in guild command for vampires
     fixed autologout
4.01 fixed owhere
     some secure improved
     gain hp/mana/moves now every 1/4 tick
     advance other now only 1...108 levels
     offline password disabled for immortals
     "charge" skill added for mages (c) Imp
     fixed gain_stats fixed for overlock
4.02 Dexterity checks in fight.c are less affectable now
     config tickstr added
     little fix with send_news
     fix send_news with murder
     curse fixed
4.03 birthday rewritten
     global bd added
     backstab changed (dex check)
     fixed charge for other classes
4.04 some fixes with converting hp
     some log fixing..
     autolook fixed
     backstab checks improved
     backstab now checks on sanctuary
4.05 colour for mobs and objects
     min hr/dr for mob =level/4
     getexp&channels for newbies normally on
     armor,bless,sanc,shield,frenzy,giant,haste,fly,invis now have level victim
     fixed getexp
4.06 spell code with victim level optimized
     second remort disabled
     advance self now works for 110 levels
     ahelp race prints in two rows
     rewritten vampire features - depends on spec_vampire, not race
     ahelp vampires improved
     vampires hand damage - punch (races.dat)
     fixed do_ear message
     add wrong players name margo kerdan...
     vampires in guild gets drain hand damage at night
     fixed mist (troubles with chances)
4.07 many spells upgraded (inform works better)
     many warnings fixed, optimized find command in interp.c
4.08 fixed vampires hand damage
     add skill info to showskill, showprac
     changed race category bonus
     improved weather, terrain bonus for skills/spells
     breathes and chain lightning now can maximum 2 charges
4.09 race penalties don't multiple with remorts
     fixed echo message TO_ALL about item affects removed
4.10 fixed dual shield block, female on male & male on female
     now cast charm better
     2,3 remorts now avaliable
     fixed bs without weapons, crush now does more damage to small
     then to huge(was opposite)
     fixed bug with 2,3 remorts (CVS commit without checkout over changes)
     fixed do_who, title limited to 40 symbols
4.11 fixed mist
     %d in prompt - day status
     resurrect now can't to ressurect zombie's corpses
     magazin improved (function clasname without spaces)
     add check for shield in do_dirt
     Item_scuba added for UWATER.(Spell wbreath vse eshe glu4it)
4.12 wbreath fixed
     mist fixed
     some affects checks removed
     wbreath added to dispel magic and cancellation
     added do_setcurse for God
4.13 little mist fixing
     global quest working version (C) Saboteur :)
     ensnare changed (added check for fly&saves)
     wbreath for all :E~
     INT&WIS now affected on do_cast - ORCs MAGEs will cry :)
     changed cast_rate(c)Apt
     dobavil aqualang kozlu
4.14 improving global quests
        immortals can create and stops global quests
        automated global quests beta version apply (global quests - on/off)
        members of current global quest is safe
        gquest statistic applied (hidden mode)
     added tar and rar feature for players file
       (tested under win32 only - rar must be in path)
     add new field in act - im_oy syntax for russian
     add magazine gquest event
     Sab
     spec_uwater added to a lot of races (c) Gendalf

     cast_rate changed again (25 average) -> less RAKA with loss of concentration
       (c)Aptekar

     spell HOLY WORD now halfed moves & hit -= ch->level * 2 (c)Aptekar
     gate and summon will not accept for gquest mobs (c) Sab
     check for time remmed in xp_copmute (c)Magic
     check for sex in clanrank (senior/seniorita), cosmetic (c)Magic
4.15
     clan channel allowrd under NO_CHANNEL
     fixed global quest - correct finish with timeout
     fixed oreset with inventory wear-location
     two_handed weapon now damage + 10%
4.16 add spec_nolostexp
     add kender for Tass
     global mobs will be updates every 3 ticks
     add spec_waterwalk
     add spec_rdeath
     pray now add 50 alignment
     do_bash upgraded (small race cann't bash larger)
4.17 affects reoptimized for placing with 32bit
     dwarves rent flag moved to Z from ff
     add rem_mist for non-vampire mobiles
     dwarves and assasin accounters moved from ACT to OFF
     ensnare affect warning fixed
     
     crash with unread_updates fixed
     fear with berserk - 5%, without - 7%
     fixed bug with fly (negative time)
     changed protective spells
     remmed protect GQ_JOINED chars from murdering
     first life - +1 additional primary stat
     cast_rate changed again (for stat > 30)
     fixed 31 stat in app_tables
     WAIT&DAZE_STATE for Zombies in begin of fight
4.18 add lash skill (lash with whip) Sab
     charge counter calculating changed
     Uwater&Wbreath are now like in best Paris homes! po4ti %)
     Fixed get crim by curse(davalos ne tomu) (c) Anonymous
     Ethereals now FLEE throw closed doors! (by flee command)
     no est li u nix spec_passflee ???
     fixed fear (curse & steal now do not begin fight) Magic
     Whip changed : now it cann't parry & cann't be parried (mojet glyuchit')
4.19
     spellaffect implemented (beta) (C) Saboteur 8/11/2000
4.20 do_dig
     censored is denied for <5 level chars
     spellaffect upgraded (do_aff, spell_ident)
     +body -mind for male
     +mind -body for female
4.21 backstab allowed only with dagger, sword, spear
     zombie max dex +2
     fixed ident, lore, objstat for spellaffect
     weapons fixed (staffs now spears)
     group level restrict removed
     fixed xp_compute for sleeping chars
     rescue updated
     global_cfg now uses flag mode
     check pk_range in do_rescue remmed (is_safe working) (Magic)
     fixed spears (spears now not exotic)
4.22 fixed do_referi with spellaffects
     new lash by Kaa
     Great bug with parry fixed
     trip halfes victim->move
     it's harder to block flail (chance /= 2) now
     lash changed again (WaitState 2 if miss) (Aptekar)
4.23 Toxic removed from is_same_clan
     gate spell fixed - no gate on plr with nosummon
     criminal fixed
     5 remort implemented (unlimited :)
     5+ remort - 1000 exp/level
     little criminal fix
     fixed bud with exp (Magic)
     fixed spell_gate
     lastremort added
     reboot make option added
     some little fixes
4.24 fixed gate
     mass_heal/invis has WAIT_STATE = 1 now
     race special +7% backstab
     some changes in log
     add time to olc.log
     short time format
     logchar for search mode added
     +2 con for 5remort
4.25 add savelock flag for areas
     mass_heal&invis have WAIT_STATE = 1
     fixed gate (can gate na klanovikov & suprugov , vozmojno i na peta)
     pray changes alignment v zavisimosti ot real_alignmenta
     gate may be FIXXXEDDDD (c)Magica
     skill strangle added (force sleep) (c) Gendalf
4.26 exp_per_level for 5remort and lastremort fixed
     removed check_criminal on success stealing
     some check_criminal fixes
     hide now has Wait_State 
     cleric's & mage's casts devided finaly
     magic_missile , holy_word , ray_of_truth , resurrect fixed
     seen fixed
     check level for gate fixed

     races for pitlord and neec are renamed
     crush is fixed (1d3 waitstate for victim, some better chances to crush,
       more simple formula to calculate the chance)
     ADDED FLEE & BACKSTAB TO MOBS!!!!
     * also i wann to add them IQ to open the doors, it may be done soon
     (c) Lina

     flee-backstab is fixed for cases with no weapon to backstab, NPC rescuing
     the PC master, impossibility to mobs to flee in selected directions and
     some other stuff.... i'll pray that all of bugs in this Artifical Idiot
     are  catched up, but ....

     little syntax in quest.c
     fix crash in spell_sanctuary string 3434
     fix ostat for spellaffects
     add deaf and quiet to command table
     little fix in is_affected to prevent crash
     stranfle - added checks on hassan, questmasters, immortals
     Sab

     some fixes to Strangle, some more needed(tam est comment 4to nujno dobavit)
     sam ja eto poka dobavit nemogu %(    
     Genda
     
     petition accept in chaos automatically raises player to "second"
     Seconds in chaos named "Knight" and "Lady"
     load random fixed, howl improved
     
     Skill 'frame' added, but don't tested too much yet.
     Practicing rate will be reduced by 1/4, due to chenging of max prac rate
     af all skills to 60%. Lina.

     crash with paf->type == -1 in spell_observation fixed.
     Read comment, plz. (uni)
4.27 Some changes in quenia
      Now chars can remember personal quenia words and use them when they want
      personal quenia words saving in pfile. Current list you can see by 'inform quenia'
     Added item type ITEM_BONUS
      item_bonus with value0==1 is books, with personal quenia words bonus. OLC changed.
     Sab

     little fix quenia, and added 'stat quenia <char>'
     charge gsn fixed (check_improve and some bugs)
     try to fix deaf
     maxrun option added to config
     Sab
4.28 Fix strdup

     STAT: Fixed.
     APAR: #0  0x3d3d3d3d in ?? ()
           (summoned with `help r')
     PRIO: Major. Somebody can easely write an exploit.
           Don't you try to look for it in www.rootshell.org yet?
           May be I'll write one ((:
     EXPL: Exploit based on fidding stack frame with proper data.
           This could be easely done by talking to someon in lostlink:
           > tell InnocentBoy {C{W{R{G{b{w{D{G... (colour expantion caused overflow)
           > tell InnocentBoy <NOPs><short_rel_jmp_to_skip_junk>
           > ...
           > tell InnocentBoy <NOPs><put_shell_code_here><shell_code_address>
           It's not too hard to calculate optimum size of the buffer and then
           "shell_code_address", which overwrites function return address 
           (remember 0x3d3d3d3d). Anyone can determine it's value from OS-type 
           and take aproximately - buffer is big enough and filled with NOPs.
           It's enough to point to only one of them.
           When InnocentBoy return from lostlink he will get his REPLAY buffer
           pasted in.
           (all this in case `scroll 0')
     Don't write things like Lope...
     (uni)
     
     STAT: Ugly fix for gquest.room[] overflow.
     APAR: full.log: [*****] BUG: Bad sn 10 in get_skill. 9515
           (gsn_dodge, gsn_recite follows the gquest static info and was 
           rewritten by room number 9515 (somewhere in New Thalos), thus 
           that skills doesn't work properly)
     TODO: increase enthrophy in rooms election.
     (uni)
 
    Gate on mobs in fight && charge for spells_no_dual fixed
    random_shield added, load genda [lvl] (c)Gendalf
    Spell_portal, spell_nexus fixed (analog gate) (c)Magic

    SIGSEGV in do_affects, caused by `order all affects' fixed.
    `affects' is now NOMOB.
    (uni)

    curative and attack groups and spell on them are now unavailable to warriors
    frame is fixed. backstab is upgraded
    body is divided into fortitude and curative now
    vampires are now fixed a little (time check)
    (Lina)

    SIGSEGV in spell_nexus, caused by uninitialized `to_room'. Fixed.
    (see compile warning) (uni)

    SIGBUS in printf_to_char, caused by `stat aff someone_with_spellaffect'.
    Fixed - another `paf->type < 0' checking added. (uni)
                                    
    Tra&Pra transfers to next remort, quest convert removed
    Char after remort have 1 tra on 5 levels

    Bug with random crap in wiznet fixed.
    How do you think it has to work? (((% (uni)

    Fixed OGROMNIY BUG s otricatelnoy exp pri generate char. (Gend)

    Stop spamming logs with unequip_char BUG messages (uni)

4.29 !!!!MINIARMAGEDDON!!!!
     New Char Version 18
     Gold, Silver, Bounty, Account = 0
     Black Crystals, Diamonds and Objects with cost > 1000gold and < 0 will be extracted
     Da pribudet so mnoj bog, ne pominajte lihom, Magica 
     one_hit changed (dobavlena proverka na gsn_backstab v checke na STR)

     APAR: memmory blocks screwing. 
           (extrem unstability after `snoop' command in some cases)
     STAT: TEMPFIXED
     This fix is only for testing/tracing.
     Real location of the bug is in feeding data into outbuf in
     EncodeXXX() routines (charset.c). They shouldn't relay on
     NUL strings terminators.
     (uni)
     Above is mostly for TODO section (;

     parts removed from chars, mobiles and mob_indexes. only race_table[xx].parts used.
     areas now has version 2. Old areas allowed.
     Sab

     Murder in ROOM_LAW is not alowed by LAW :).
      Check_criminal in this case doesn't check fol clan->war.

     Now holylight is works for dark rooms.
     DELETE command do not backuping chars younger 25 years to the DELETED folder 
     removed old_format mobiles (useless)
     removed mobiles material
     fix warnings
     add nomob to many commands into interp.c
     in do_interp mobiles now has maximum trust - 101
     Sab

     channel ALLI fixed (APTEKAR)

     add sset command - low level immortals can change them parameters. not others
     Sab
     Some stone's like flaming, sharp etc FIXED.

4.30 orc's crush fixed
     summon cursed victim fixed
     only pierce damage allow to backstab
     nosuicide room flag added
     fixed backstab - backstabin with right weapon
     Sab
     fixed bug with asave, now all areas is Version 2; Magic
     vampiric_mana flagg added for weapons
     fixed gtell in army
     fixed check_improve for pray

4.31 quest cancel now removes 1-5 qp
     added Druides Guild
     Dwarves rent uses character account gold. If account=0, uses character gold.
     Sab

4.31.1 (c) Apc
     *  Dissalowed entering safe rooms (and gate there, but summon is possible)
     under adrenaline.
     After arenas adrenaline is set to 0. Otherwise, players can't leave room :)
     In check_criminal there is now return() if $ch is in ROOM_ARENA
     *  Added blink mage skill (1 tra worth 1 level). Isn't cumulative with dodge
     (is blink is active check_dodge returns zero)
     *  Added Shield Cleave skill for warriors (38 lev, 4 tra)
     Only axe can now cleave the shield.
     * maximum remort is now 2 (i.e. no more Bodhisatva's & Heroes).
     But players can now change classes (i.e. drop one and take other).
     * improved  thieve's strangle skill (See code - it's rather large).

4.32.A (c) Apc
     * Detect/Undetect levels added
     * Inform changed
     * Offence inform added 
       Added offence to all offencice skills (axe, disarm, etc..)
       Added offence to all protection checks in fight.c
     * If player doesn't have gold, he's trown out of the DRENT room
     * saves changes, effective saves are now /2
     * 1 imm added :))
     * fixed bug with fireshield (gotta remake all the dam_message, 'cause 
       it's SO UGLY!!)
     * added variable FD_VERSION in merc.h and rewrote the version func.
     * BH's are not supported now :) added lowering to last remort in save.c
     * is_safe and is_safe spell are changed a bit (now NPCs can really do more)
     
     * some more added, but I've already forgot.. :(((

4.32.SP1
     * many inviso/etc bugs fixed
     * now everyone's visible by 'who', 'whois', and in common channels
     * detecting system is fixed: the chance to see is now 
       100 - (cast_lev - det_lev) *100/6 (idea by Sab )
     * kill/deats stats are now ONLY 4 imms
     * quest stats - only 4 yourself (both ideas by Gendalf)
     * fixed back guild inform
     * sneak is now only for coming into room, and hide is only 4 look/scan/where
     * act() fixed - no more 'HekTo's   
     * as always, some more added, but.. :~=((
     * added room flag ROOM_ALL_VIS , and checks in can_see (4 detecting chars only)
     * added constatnt MAX_PK_RANGE
     * PK_RANGE limited to 6 levels
     * Char is not allowed to enter DRENT room without gold
     * Mobs do not flee into distortions and under ensnare
     + add flag to can_see. who and whois not show if can_see char
     * fixed some visibily bugs
     * Rename coupon Added (150qp) - allows to rename thing (using imm, of course) :)
4.32.SP2
     * wizinvis bug fixed  
     * envenom upg. 
     * imms can now remove NOREMOVE, drop NODROP, wear two-handed, etc.. :)
     * steal changed, +100% to steal if victim is sleeping, some checks fixed.
     * some more added, but i forgot again :(( (c) Apc
     * NPC can steal now :)
4.33 (C) Sab
     removed FD_VERSION bug
     upgraded foot-wrapping script
     updgraded reboot command. Now allowed to checkout and compile without reboot
     fixed global and reboot command. look 'reboot help'
     removed MAX_QUEST_ITEM variable. now [level==0] is used as end of table.
     allow enter safe rooms with adrenalin
     players under adrenalin is not safe in safe rooms
4.34 (Sab)
     Add SPEC_IGNOREALIGN - humans can use items with any anti-align flags
     Zombie's Dexterity incrested by 3
     fixind detect/undetect levels for mobiles
     Reserve of file streams fixed. Now files will be created if miss.
     fixed quest items - scrolls of quenia

4.35 (C) Apc
     -=** If U Can't Understand What's below, try switchin' to CP-1251 **=-
     Запарило меня упражняться в Английском...
     
     * Пофиксен буг с кастанием poison'a/etc.. в сейф комнатах :)
     * Пофиксена проверка на safe room в do_murder :))))))
     * Пофиксен буг с Орденом - теперь они не могут брать вампирик в ПРАВУЮ руку
     * Добавлены макросы BAD_ORDEN (orden && align < 750) и 
       BAD_VAMP (vamp_guild && align > -750)
     * Добавлен макрос IS_ORDEN (на проверку Ордена) НЕ ПУТАТЬ С ISORDEN !!!
     * Ордену попоганен bless ,frenzy и prot. evil (теперь их нельзя кастать на
       других "плохих" Орденцев, и блессить шмотки (saves +1 :) )
     * Орденцы раскурсовывают/блессят свои (!!!) шмотки с первого раза.
     * Орденцы +50%lvl к RemCurse.
     * Переправлено пару кривобугостей в update.c 
       (типа if (IS_AFF (ch,..) && ch!=NULL ) :)
     * Усилено воздействие poison'a и plague (теперь бьет при каждом gain_stats() )
     * Если чар под slow , poison damage /= 2 (Haste => *=2);
       Сделано несколько рачно, если кто может лучше - welcome to gain_stats() - update.c
     * Strangle теперь качается выше 60% :)
     * В backstab добавлены проверки на offence|protection 
     * На иммов теперь не вешаются десятки pray'ев
     * Гильдийские Вампиры теперь ЗЛЫЕ (как Орден) (надо согласовать с Каа, что делать)
     * IMMS PLUS PACK v0.99 :))
       + расширена addpenalty (addpenalty без аргументов для справки)
       + убран addpenalty list (перенесен в addpenalty <noarg>)
           полезно было б вместо него сделать список текущих пенальтей, 
           все равно в МУДе больше 40 игроков одновременно не бывает.. 
           (ск. всего сделаю в след. версии)
       + Добавлен IgnoreList - действует как vislist, но наоборот :) (106 levl)
     * Добавлен smash_beep в do_title() :)
4.36 (C) Sab
     fixed warnings in update.c
     fixed crash with affects in update_stats
     all liquids names fixed to english state
     add fix_liquid table - for renaming liquids, add function to liq_lookup
4.37 (c) Apc
     * Sab fixed :)) 
        Mob invis/hide fixed
        Unicorn table fixed
     * IMMS PLUS PACK v0.99.1
       + P_NOSTALGIA added - NEED EXTENSIVE TESTING !!!
     * faerie fog is now roomaffect (makes ROOM_ALL_VIS)
     * P_DENY fixed - теперь по истечении срока выкидывает из игры
     * quest convert is back (50+ level)
     * pk range is now 6  vs 1st life , 8  vs MLT
     * some tweaks in gain_stats
     * added smash_newline func to db.c and to do_title()
4.37.1
     * smash_newline в title какого-то Х не работает 
        (так же как и smash_beep и проч. - мож я не дошарил, как ее юзать?)
        кто знает - плз посмотрите.
     * Пофиксен буг с ESC последовательностями - теперь ВСЕ ESC последовательности
       от чара заменяются пробелами - должно появиться куча новых бугов, но я пока 
       не заметил. Цвета, вроде, живы, потому как не ESC.
     * Пофиксен краш в marry <char> <room> при отсутствии чара. Обнослен хелп.
     * Пофиксен небольший глюк с wizinv и vislist.
     * Пофиксен глюч в ignore list, когда help выдавался по vislist'у :)
     * Вроде отдал мобам назад emote.
4.37.2
     * УРА! Я дошарил smash_beep и т.д. :)
     * У smash_beep появился второй параметр - bool total, если равен 1, 
     то убивает все beep'ы, иначе оставляет по дефолту первых 2.
     * У divorce появился параметр force (см. help) - вычищает у чара всех lovers
      и married - для случаев, когда один из супругов поделетился (P.S. сделать-бы
      проверку в do_delete).
     * allow спущен (как и ранее ban ) на 106.
     * Dwafves - кастуют под берсерком без проблем.
     * пофиксен буг с is_safe_spell ( теперь уж ТОЧНО poison под хилером не вешается).
     * в кланах теперь качаться нет толку - эксп не дают.
     * ограничения на vote (по возрасту/level'у)
     (Sab) little fixes with proto.are and null.log, also fixes in repository
     (Sab) upgraded winmake.fil for clearing before commiting
     (Sab) fixes with offline compiling
     (Sab) add resurrect for druid guild
     (Sab) fixed some troubles with save chars
     (Sab) fixed bug with crash in talk()
     (Lina) small play-testing feature - temporary recall denying for mature chars,
     (Lina) leaving it newbie_only. Other recalls - arena, sprecall, clanr - accepted
     (Lina) from midgaard only. We will see, how it works and if it will improve
     (Lina) gameplay. All changes are marked with /** Lina **/ mark, you can easily
     (Lina) undone it. but i nedd a some time for test - a week, for example, and then
     (Lina) a vote for results.
     (Sab) some fixes surround time-penalty feature

4.38 (Sab) Merc.h now divided on merc.h, merc_def.h, merc_str.h some optimizing
           with defines, and moved from many files to merc_def.h
     *     removes starup and starfd files - now only foot_wrapping script
     *     removes proto.are and null.log files - now created if not exist
     *     add make del for winmake.fil - clear all unused files
     *     rewrite reboot_counter feature - shutdown mud if crashes very often
     !     Пожалуйста пишите в дальнейшем изменения в таком стиле - в скобках
           автор, затем текст (не превышая 80 символов в строке). Отдельные
           пункты помечать значком, продолжение текста без значка. Русские
           комментарии в win-кодировке, английские - максимум простоты для
           удобства читающих со словарем :). Я все сказал. Саботер. :)
4.38.1 (c) Apc
     * Saves перефиксены. Теперь по aff пишется эффективный савес, а в скобках
       saving_throw
     * Allied клановики теперь друг на друга нападать могут.
     * Пофиксены немного детекты.
     * Мелкие фиксы повсюду, где заметил.
     * Иммов не видно по detect good/evil :)
     * divorce force fixed
     * Assasin guild info removed from whois (ибо достали)
     * Immortals now can tell to chars in army being in other areas (ибо тоже достало) 
     * Some safety checks in quest.c
     * Criminals are now not safe in ROOM_SAFE (вообще, их теперь бьют все и всегда!)
     * Мобы не входят в порталы, если с другой стороны NO_MOB
     * Во всех fread_xx() заменены exit(1) на return NULL 
       + теперь не крашит при неизвестных внумах, попытках загрузить несуществующую арию
       - при неКонченых ;) плеерах зависает :( 
       - надо тестить на разные комбинации бугов.
     * После смерти адреналин ставится в 0 (иначе сам понимаете.. :) )
     * Пенальти теперь сейвятся и лоадятся из файла (по аналогии с аффектами)
       penalty->affect is now int (not int64)
     * При убийстве чара чармисом или петом ухо/баунти/статистика дается мастеру
4.38.2 (c) Apc
     * Distortion вешает на чара соотв. аффект не дающий снова кастать distortion
       duration = ch->level/10
     * У воров улучшеный detect_hidden (level = UMAX(ch->level + PERCEP, cast_level)
     * some bugfixes
4.39 (Sab) add rlist command
     * little fixes with compile script
     * sprecal, clanrecal allowed not only Midgaard, but in SECT_CITY rooms
     * alist command upgraded - can use
           alist <fromvnum tovnum>
           alist <filename prefix>
     * some fixes with marry_room - now sprecall allowed for non-marryed chars
     * room_all_vis added to tables for OLC ( Apc, RRRRRRrrrrrrrrr )

4.39.1 (Apc)
     * Distortion нельзя кастать в кланах
     * Низзя одевать два щита одновременно
     * Под адреналином нельзя quit
     * Бугфиксы.
+---+ (Magic)
     * Sacrifice now extract objects from NPC corpces to room
4.40 (Sab) some optimizing code with next new function:
     * added raffect_level ( room_index_data, raffect_bit)
     * added get_raffect ( room_index_data, raffect_bit)
     * add 'oasis' spell - increase heal rate in room
     * add 'mind channel' spell - increase mana rate in room
     * removed ignore_align feature with items for humans
4.41 * add variables 'mother' and 'father' for PC
     * add family command with options: room,marry,father,mother,lovers,propose
     * add noident item flag
4.41.1 (c) Apc
     * Вашу Мать - проверяйте свои чанджесы хоть иногда! 
      (особенно относится ко мне и Сабу)
     * Пофиксены неработающие (!!!) mind channel и oasis
     * Изменена структура update.c (в сторону улучшения читаемости)
       ВНИМАТЕЛЬНО ПОСМОТРИТЕ ФАЙЛ.
     * Мажеские спеллы чекаются на инт. Клерические на виз. (см. do_cast, ищите
       CLASS_CLE)
     * Пофиксен БУГ с инвентори под blindness
     * Есть идеи попрофайлить {mobile|char}_update - обьединить в одну и укоротить
      жду отзывов в мыле.
     * Мелкие бугфиксы.
     * Пофиксены ухи в инвентори (теперь за вещь не считаются)
     * Под holy light видно evil/good
     * Можно драться под invis

     (c) Magic
     * fixed bug with time to next quest on 'remort'      
4.42 (c) Sab
     fixed clone mobiles crash bug
     add logline with NULL in fire effect
     room raffect now shows header, even no raffects in room
     room clear - even no raffects in room, shows notification
     fixed raffect names in some commands
     fixed global update - now prefixes are works
     (c) Ast
     spell "evil presence" implemented for mages: sets raffect no_recall
     added uppercase to "father" etc. in whois
4.43 (C) Sab
     remmed "father" in whois
     fixed bugs with recal, clarecal, sprecal (Astellar - get highlight at eye)
     Astellar removed from 110-level check
     Fixed 7 bugs in spell_evil_presence() (RRRRrrrr...zareju)
     Fixed bug in raffect_update()
P.S. Люди, когда пишете что-то в test.c старайтесь уложиться в 80 столбцов, не
      ленитесь перенести на следующую строку или перефразировать фразу в
      предложение с более понятным смыслом.
     Кроме того, не забывайте, что кроме test.c во время коммита тоже следует
      указывать что коммитится, и в обязательном порядке ставить имя.
     Тестить все фиксы на локальном муде очень и очень поощряется.
     Саботер.
     (c)Magic
     quest convert removed ;E
     (c)Ast (sorry for bugs :( second commit hadn't come)
     Spell_evil_presence() fixed with its raffect->level;
      added right :) check on it in portal, nexus;
     Spell_life_stream() for clerics - removes RAFF_EVIL_PR,
      allows recall and clanrecall (duration 0 ticks);
     These two spells are availiable at 65 level.  
     Spell "magic web" fixed a bit:
      Because it had put RAFF_HIDE in exchange of RAFF_WEB
      fixes in do_flee with "magic web"
     Brackets fixes :(
     'evil presence' depends on DARK only, 'life stream' on LIGHT only
     (C) Sab. Fixed recall, sprecall, clanrecall with life stream
4.44 (C)Ast
     Полностью переделан 'shield cleave' (Арс - оторву руки);
     Skill 'shield cleave' now depends on FORTITUDE
      (chance += category_bonus(ch, FORTITUDE));
     Имморталы и берсеркеры не подвержимы страху;
     Усилена зависимость 'berserk' от FORTITUDE
      (добавляется к хр, др тройной бонус фортитуда);
     Начат процесс привинчивания obj->condition, в связи с чем поле состояния
      добавлено в 'lore' и 'identify'.
     (C) Ast
     'evil presence' duration decreased,
     char already affected by 'life stream' cannot cast this spell,
     check_improve added for these spells;
     shock_hit fixed(!!!) 
     Blin kto eto delal???? char bez skilla mog ego usat'...;
     added waitstate to 'recall', check_improve added for it,
     pri recall iz boja zabiraetsja 100 exp;
     lash skill fixed.
     'gate' spell depended on MIND;
     'fear' for berserkers fixed... at last...
     cosmetic fixes :)
     (C)Sab:
     fixed bug in act_move (arecall)
     added new function of create_random_armor by Gendalf
     (c)Ast:
     Поставлен WAIT_STATE для strangle;
     clanrecall, arenarecall, sprecall зависят от скилла 'recall';
     Поправлены parry-, dodge-, blink- проверки;
     'Искажение пространства' зависит от РАЗУМА,
     Уровень 'distortion' повышен;
     На время возвращена старая процедура рандомов;
     'pick lock' зависит от интеллекта и ловкости;
     'evil presence' при кастовании вешается и на чара;
     Cosmetic fixes.
     (c)Apc
     rename moved to 106
     (c)Magic
     rename moved upper to 107
     
4.45 (C) Udun
     clan.c(do_petition accpet and remove)
     Group of clan skills automaticly adds to char when his petition accepted
     and removes when char has been removed from clan.
     guild.c - previous works when Immortal do "guild" command.                     

     Added new clan "Cyborgs".
     Cyborgs cant cast ANY spell.
     Firearms(new weaponskill) skill added to "Cyborgs"
     Skill Rifle added to "Cyborgs" - make Firearm-weapon from any.
     Skill ForceField added to "Cyborgs" - deflect spells
     Skill Dopping(speed,str,accuracy,vision) added to "Cyborgs" - haste,
                                                   gia str, bless,detects
     Skill Resistance added to "Cyborgs" - sanctuary                                                            
     Skill Bullet Strike added to "Cyborgs" - increased damage
     Skill Autoreapir added to "Cyborgs" - heals and adds regeration affect
     Skill Headshot added to "Cyborgs" - chance to damage half-hp of victim on 
                                         kill/murder+ chance to blind + chance 
                                         to weaken.
                                      
     Added new clan "Skywolves".
     Spell Safty Place added to "Skywovles" - make room safe, remove adrenolin,
                                              stop fighting.                                  
     Spell Iron Skin added to "Skywolves"  -  -AC                                          
     Spell Inspire added to "Skywolves"    -  -SAVES
     Spell Rejuvinate added to "Skywolves" -   restore
     Skill Healing added to "Skywolves" - cure blind+cure poison+cure disease
     Spell Peace added to "Skywolves" - stops fighting, cant attack if affected
                                        by Peace, removes adrenolin.  
                                     
     Spell Violence added to "Dark Angel" - removes Safty place or SAFE
     Spell Spook  - forces victim flee and adds fear affect
     Spell Madness - +HR,+DR, attack all that can see                                                                 
     Spell Death Eyes - shows room where victim
     Spell Pain - halfs hp
     Spell Breath of Death - damage + chance to maladictions
  
     Spell Cursed Lands added to "Night" - NO_RECALL to area     
     Spell Night added to "Night" - BLACK CLOUD to area     
     Spell Power of Darkness added to "Night" - +DR, protection good, -AC
     Spell Dark Swarm added to "Night" - damage, chance blind, weaken, poison
     Spell Vision added to "Night" - can see in Black Cloud
     Skill Black Sword added to "Night" - upgrade weapon sword
  
     Spell Consecrate added to "Light" - removes Cursed Lands from area     
     Spell Daylight added to "Light" - removes BLACK CLOUD from area
     Spell Nimbus added to "Light" - +HR, protection evil, -AC, -SAVES
                                     can see in Black Cloud.
     Spell Shine added to "Light" - lesser chance of curse, when not fight, 
                                    adv.blind when fighting.
     Spell Turn added to "Light" - Ray of Truth + forces to flee.
     Skill Holytouch added to "Light" - remove Anti-good, evil flags from item,
                                        +HITROLL to weapon.

     Keepers - physical damage/2, reduced chance to hit.
     Skill Spellcraft added to "Keepers" - increases spell-level.  
     Skill Wild Magic added to "Keepers" - cast random offensive spell,
                                           can damage caster.  
     Spell Blast added to "Keepers" - damage + chance to blind
     Spell Magic Barrier added to "Keepers" - chances to deflect damage.  
     Spell Time Stop added to "Keepers" - wait_stat to all PK in room
     Spell Incineration added to "Keepers" - sets hp to 1, or damage if saves
  
     Skill Hidden Movement added to "Shadows" - subj.
     Skill EQsteal added to "Shadows" - subj.
     Skill Money Count added to "Shadows" - shows gold and silver amount.
     Skill Advanced pick lock added to "Shadows" - can pick pickproof.
     Skill Check Container added to "Shadows" - look in other's player container.
     Skill Cut Container added to "Shadows" - subj, 1-3 things are out of container.
  
     Skill Espionage added to "Mercenaries" - shows victim's area  
     Skill Estimate added to "Mercenaries" - shows victim's score
     Skill Tie added to "Mercenaries" - like charm but master cant order  
     Skill Weapon specializaton added to "Mercenaries" - lesser chance to block,
     parry,dodge,blink attack. Not much increased damage,chance to disarm when
     parried chance to counter_strike when parry, chance to shield cleave when
     shield blocked.
     Skill Mercenary weapon added to "Mercenaries" - upgrade weapon  
     Skill Ambush added to "Mercenaries" - wait for victim, then hit it with
                                           much damage
                                
     Channel "RP" added.
     IMM command pseudoname added - insert your string instead your name                                        
     "offtopic" added
     added several phrases for lost concentration
     liiitle fix in do_strangle  

4.46 * Fixed 2 bugs with mercenaries clan, 1 bug in disarm skill (firearm)
     * add new function - dlog - this is log_printf, wich is work only if
       global option debuglog is on. 
     * global option DebugLog added :)     
       use: dlog("update.c string 1024");
            dlog("begin of do-aff: current aff:%s",skill_table[sn].name);
     * fixed fraze in spell_regeneration
     * fixed few bugs in do_aff
     * fixed great bug in spell_bless
     Sab.
     
     * fixed bless - APPLY_HITROLL is back :))
     * fixed some ГОН in magic2.c
     (c)Apc

     (c)Ast
     Add to lash move/2
     Add check in do_berserk for move/2, when move < 2. 
     Change some string in magic.c, when concentration lost.
     "Тебе на голову упал астральный кирпичь" и др. не совсем удачные фразы,
      как и заточеный пулемет мастера...
     Fixed level of energy_drain, 132 level -> 125 level (Было +12 на 101 level)

     (C)Sab
     - fixed enchant weapon/armor and few spells in do_cast (crash)
     - add crashfix check in get_room_index (dirty :()
     * add some info for immortals in reboot status

     (C) Udun
     + added flags for new clans
     - Cyborgs renamed to Inquisitors, Cyborgs skills renamed and rearranged.
     - Mercenaries renamed to Hunters.
     - some fixes in do_black_sword and do_mercenary_weapon
     - some helps and clans.dat upgraded
     
     (c)Ast
     * percental damage-show style implemented
     * config autodamage is ignored by non-immortal

     (C) Sab
     - fixed get_random_room - violence check, random_room check
     * clanbank status now show after every put command

     (C) Apc
     * damage string depends now on CURRENT hp of victim

     (C) Sab
     - fixed "marry room" crashing bug, when char not found
     * some workaround in memory leaks with raffects, vote, vote_chars, etc.
     * warning messages in log_string marked with "BUG:" - for easy filtering

     (C) Apc
     * fixed charge - при любом количестве зарядов работал только 1
     * fixed leader remove - не сохранял чара
     - removed a lotta Udun's stuff from do_cast - и так ничего не понятно,
     так ещё и буги...

4.47 (C) Sab
     * fixed bugs in spells - someone set int for vnum (must be int64)
     + add autologin feature in config file
       autologin <codepage> <ask password>
       if codepage set, no greetins, and default codepage selected.
       if pass set, do not ask password for entering characters
       and motd do not show to char.
     * LocalMode options now saving separately from global_config
     * fixed all warnings in fight.c
     * fixed cleader bug (with victim->clan=NULL)

4.48 (C) Sab
     * security: offline deny for 110+ can be removed only by 110+
     * security: 110+ characters can be restored only by 110+
     + add pban command - personal ban system for immortals
     * fixed crashing bug in snoop
     
     (C) Invader
     * rewrite string.c (olc editor), one more command introduced and
       buffer overflow bugs are fixed :)
       rewised formatting function is coming soon
       
4.49 (C) Sab
     * fixed obj_cast_spell crashing bug
     * fixed some great bugs with char_list!
     * pban command greatly improved - IMMORTALS, USE IT!!!
     + add offline clearpban for some reasons. 110+ level allowed.
     * some leaks with free_char and char_from_room
     * src modules fixed and worket
     * romtell feature now used by offline commiting areas and compiling mud
       (send notification to immortals about finished process)
     * offline command fixed for error str_prefix usages
     * full showkills statistic now shows only for imms.
     * arena kills statistic are allowed for mortals
     * spirit bonus to sprite added
     * if char died from himself there are no events in magazine now
     * char list updated in new_char and free_char

    (C) Invader
     * do_who, do_count revised, now it is impossible to work out if immortals
       are online from visible/all characters counters. those functions are also
       tested and cleaned up, so they are much much more readable now :)
     * GREAT BUG FIXED!!! nanny function is beautified (however there is much
       work to do there), check_reconnect is fixed, names checking is changed.
     * update characters that are not in play does not put them into limbo, so
       newly created chars are no more put into limbo while customizing
4.491
    (C) Saboteur
     * temporary fixed hunger_damage bug - checking  on room_vnum 3
         without check on NULL.
     * fixed talk() and other function with checks on descriptor list
     * fixed crash in act()
     + Wagner add to imms check
     * offline clearpban uses save_one_char - not save_char_obj
     * fixed arena statistic for immortals. Some corretcions in whois.
     * now social to sleeping chars correctly show victim's sex
     * stat mob now show victim's sex correctly.

    (C) Invader
     * clanbank bug fixed - put now always taxes the payer and actually
       DOES it :)
4.492
    (C) Saboteur
     * damage messages now depends on damage, not percent. 
       dam_msg_table created. Numbers shows to immortals.
     * ahelp damage option added. Numbers shows to immortals.
     * Fixed Money Count

    (C) Invader
     * damage messages are beautified, they contain much more sense now
     * Percent of the fighting opponent are shown and option to show them
       in prompt is added
     * damage messages are fixed (some bugs introduced by Sab :)
     * quick fix for do_offline, but it seems more bugs are still hiding :(

    (C) Saboteur
     * wizlist corrected. Invader added :)
     * some fixes with descriptors. get_online_char fixed
     * do_offline fixed (some checks was without extract-char)

    (C) Invader
     * fixed bug in do_offline after Sab *sigh*

    (C) Saboteur
     * fixed gecho :)
     * added autonote, autovote and autoplr options in config
     * connected characters has only 2 ticks to enter password, then disconnect
     * ochepyatka before (logout) in config fixed (10 qp to Apc :)
     * some crash bugfixes
     * do_practice - list of practices upgraded

    (C) Apc
     * fixed char_death - now properly updates conditions.
     * <note> read -xx implemented
     * save_char_obj now castrates non-imms only :)
     ! tra hp/mana is now gone!
     * Вход в гильдии (CLE/MAG/WAR/THI_ONLY) под адреналином запрещен.
     * сделаны две удобные функи whois_info и mstat_info
     * фиксы в fight.c
     * фиксы Сабовских варнингов
     ! СИЛЬНО расширен do_offline (но не сильно потестен)
       добавлен offline delete <char> - херит чара из Player и backup
       (только под unix) - НЕ ВИДЕН ПО offline help (dumb anti-cheater)
     * offline load - грузит чара в лимбо. offline unload че-то глючит :(
     * что-то еще.. короче - plus pack :)
     * после clanbank put показывает, сколько теперь на счету.
          
4.493 (C) Sab
     ! tra hp/mana restored.
     * после clanbank pug - фиксед qp
     * level checks for immortals removed from blacksmith
     * fixed bug with guild entrance with adrenalin 
     * fixed money_count bug with null clan thief check
     * add alert to telnet_copy with null destination
     * some fixes in telnetcopy
     * add olc features: delete and undelete rooms
     
     (C) Apc
     * guild entrance bug finelly fixed (did allow entering any guild)
       now adrenaline check is correct
       also, non-pets cannot enter guild rooms
       
4.50 (C) Sab
     + add few new npc races - golem, elemental, evil.
     * removed MAX_RACE definition. Now check uses last null data in race_table
     * spellstat now shows % of spells learned. Little improved colors
     * room undelete command now works
     * fixed updating characters, that are not playing
     * fixind killing statistic
     * autoremove room_deleted flag from loaded rooms
     * family command improved - info added, help changed
4.51 (c) Apc
     * pseudoname fixed
     * added check is_safe to one_hit (to fix ranged weapons)
     * do_practice enhanced

4.52 (C) Sab
     * fighters can't see zritels on arena
     * immtalks for not connected chars fixed
     * greatly improved show skills in do_practice
     * allow for some another item_type to be sold at auction
     * telnetcopy - another fixes
     * now level 5 allowed to tournament
     * fixed ahelp race

4.53 (c) Sab
     * some issues with force commands fixed. NOFORCE flag added to cmd_table
     * bugs with buffer fixed. Charset must be fixed in future

     (c) Apc
     + Added Orden clanskill - pacify
       * acts as peace with a chance and on non-berserked aggrs only
       * removes aggr flag & stops fighting
       * doesn't work when fighting - should move in tank first
        (maybe, this is excessive)
     * Strangle is now 0 ticks long (as dirt)
     * Percent coloring in do_practice added

4.54 (C) Sab
     + spell observation now can show room affects and some info
     * TAR_CHAR_ROOM spells type added
     * races added: Plant Rat Gargoyle Minotaur
     * try to fix classname bug
     * bless stone add permanent saves for item.
     * MB fixed bug with incorrect close_socket of char with overloaded buffer
     * random armor now types 1..9 (no firearm now)
     * close_socket now works better, also removed makintosh defines
     * random weapon levels now 0..101 not 10..101

4.55 (C) Apc
     * ears in containers now are not counted in num_carried
     * magic web fixed (IMHO) - lotsa fixes through all the MUD
     * life stream now has the chance to remove magic web
     + "Avtoritet'" - when checking aggr's attack - +vch->criminal/50 to victim's 
       level         
     * changed load command - now load r_weapon & r_armor     
     + one_hit returns if ch->position < POS_RESTING
       check_blink/shield_bl returns FALSE if ch->pos < POS_RESTING
       check_parry/dodge returns FALSE if ch->pos <= POS_RESTING
       (thus disabling hitting & blocking by stunned/etc chars)
       
     (C) Sab
     - little try to fix crashes... :(

4.56 (C) Saboteur
     - Fixed bug with crash in descriptors :)    
     * do_whois added check for wch

4.57 (C) Saboteur
     * immortals can see auction info about seller and buyer
     * spellstat fixed (now shows VICTIM's learned stats) 10nx to Wagner
     * many recycling fixes and optimizes
     + mem command rewritten
     - fixed free_descriptor function (mdaa..)
     - little try to fix reconnecting at remort
     + add cnt_xx counters, shows them to mem

4.58 (C) Sab
     + enchant weapon - now change obj level more than 101
     + makemastery*5 chances, that enchant weapon do not add level to object
     + makemastery*5 chances, that enchant armor do not add level to object
     - fixed memory leak with buffers in do_socket

4.59 (C) Sab
     - Fixed bug with 'leader remove'
     - char_list and descriptor_list updates now must work better
     - YES! Crashes fixed!

4.60 (C) Sab
     * Druids guilds has +1 to curative, Elders has another +1 to curative
     * Dwarves guild has +1 to makemastery, Elders has another +1 to makemastery
     * shortflag of hidden now is (C) (Скрыто), not (У)
     - fixed bug with autologout
     * Vampire guild +2 evil, Elders +4 evil
     * Vampire guild -3 light, Elders -6 light

4.61 (c) Apc
     ! Damage calculations fixed
     * Blink fixed
     * Charmис left in group fixed
     * Если чара убивает чармис - ухо не дается.
     * Иммские дамаджи не зарезаются :))
     * У традеров (всех) act_nosteal
     ? Пока не пофиксится адреналин (хотя, должно уже), армейцев кидает на 
       армейское кладбище.
     * Limbo под адреналином не работает.
     * При убийстве показывает, была-ли жертва была в лостлинке.
     * Cleav'ают теперь только axe & mace. Nodrop падает в инвентори (иногда)
     * нахимичен do_disarm.. :)
     ! COOL ADRENALINE BUG FIXED !
     ! PLAGUE & POSION BUGS FIXED !
      
    :) Bug in test.c fixed!
     
     (C) Sab
     * dendroids align now ajust to char align.

     (C) Apc
     * more blink fixes
     * cleave fixes
     * dendroid align == 0 (wood doesn't care)

     (C) Sab
     * some cosmetics
     * oset now allowed for all imms
     + socket command rewritten. argument 'all' added
     ! fixed bug with remove charm affects (charmis left in group)
     * little cleaning of useless log messages

4.62 (C) Sab
     * counting characters in socket added. cosmetic fixes.
     * newspaper command rewritten. now filtering working with x, -x args
     * some debuglog useless messages marked as remarks.
     * mag list <x> filter - counting not filtered number of first event
     * fixed - character stays in group after charm is fade
     * more command removed from automated logging
     * little fix in rename.
     * add deaf parameter to slay quite
     * removed LUCKY_NET defines
     * add checks for 109 level
     * now only wealth races has gold/silver
     * do_race beginning to be editable. added wealth command
     * fixed who_name - now all names has 8 symbols length.
     * removed lostlink messages, when rebooting or quit

     (c)LastSoul
     // My first commit
     + Note about Blacksmith using. Improved for better Guild control
     + Guild Account information can be retrieved from any room
       (account operation can still be performed near keeper only)

4.63 (C) Sab
     * some security added
     * some fixes and optimizing in many functions

4.64 (C) Sab
     + spell mummy added
     * morphed characters now can see gate, summon, teleport and transfer of
       masters, who carry them
     * rewritten races workabout. Now race_table is EMPTY! only races.dat uses
       be carefully
     * race command greatly updated. see new help
       added name, who_name, wealth, hand_damage parameter editing.
     * removed useless gsn_numbers
     * gsn's workaround - FD now works little faster
     * many other optimizations and changes

4.65 (C) Sab
     * race info - added hand_damage msg
     * add push command - moving objects
     * mummy spell - some fixes with npc.
     - removed macintosh code
     - removed msdos code
     - removed amiga code
     * many code reformatted=======
     * add push command - moving objects
     * mummy spell - some fixes with npc.

4.66 (C) Sab
     + add new quenia word - peace (stop all char, that fighting with ch)
               power peace (stop all fighting, remove adrenolin and aggr)
     * little fixing in pseudoname
     * fixes do_sit, do_rest, do_stand.. my sorry
     * removed q_name function. added long quenia name to quenia_table

4.67 (C) Sab
     * death eye spell fixed
     * clan_spells are not allowed in random objects
     + clan spells&skills feature added. Now only in manual mode.
       clanwork info, clanwork addskill, clanwork remskill added
     * little changes in clanwork syntaxis
     * clanspells allower for use:
       "death eyes"       - to see the room, where victim is
       "death blow"       - critical strike (skill)
       "vision"           - can see in dark
       "cursed lands"     - curse whole area
       "eqsteal"          - steal equipment
       "consecrate"       - remove cursed lands
       "pain"             - -1/2 hit from victim (if victim is healthy enought)
       "madness"          - +hr/dr, autokill all in room
       "safty place"      - make room safe, or remove violence affect
       "truesight"        - detinvis, dethidden
       "metabolism"       - skill for regeneration spell
       "peace"            - peace spell
       "violence"         - not safe place
       "check container"  - look in foreigner container
     * evil presence - removed useless affect

4.68 (C) Sab
     + add autoremoving for clanskills - now can be set for x ticks
       optimizing code, functions add_clanskill, rem_clanskill
     * add global update clan option
     * little upgradint cursed lands, consecrate
     * check_blink rewritten 
     * clanskills/clanspells add to clan status in do_leader
     * fixed consecrate and cursed lands
     * some fixes in clan_update
     + liquid "lava" added
     * leader syntax little changed
       members can see status. also loners can see status by leader command.

     (c)LastSoul
     * spell_charm will not affect sleeping targets
     * removing char from group if charm removed by cancellation
     * some messages fixed/beautified

     (C) Sab
     * clanwork syntaxis fixed (clan delete)
     * config autovote, autoplr, autonote now shows correctly
     * new_one_hit function, added to global
     * description fixed
     * removed useless gsn_tail
     * do_send - fixed calculate of correct gold and silver
4.69 (C) Sab
     * gendalf added to imm_check
     * olc_mpsave optimized. removed 3 useless functions
     * added mpedit delete option
     * recycling memory with mobprograms rewritten

     (C) Ast   : news
     * clanwork & protect raised up to 109 level
     * do_mstat beautified and corrected
     + basis for new spell- 'gaseous form'( Jet now from 102nd level)
     * imm-checks in 'life stream' and 'evil presence' - no char affects
     + new liquid 'sun nectair'

     (C) Sab
     * immortal check moved to comm.c from save.c (offline command generates
       news every time when offline info was used
     + implemented directory read structure. tested under win32

     (C) Ast   : skill, features
     ! rewritten do_blink: from now social "blink" is usable again
     * clanspell "gaseous form" is complete (I hope so...)
     + cset(set char) implemented for set pc and allowed within 109
       mset is now for mobiles only and still within 106
     + oset level generates news (deja vu :) every time it have been used

4.70 (C) Sab
     + directory read structure tested under unix.
     * removed flag_type64, flag_string64, flag_value64. All flags are now
       64 bit. Fixed many bugs with incorrect using of structures.
     - removed wear_neck_2 location
     + mpcode olc rewritten.
     * added help for mprog olc
     * removed secure set (added set self)
     * fixed cset, mset :) added checks for NPC
     * added remqp option. some set char options allowed for 106 levels.
     * may be setclass fixed
     * fixed gread bug in save_resets
     * little tmp_fixing in load_resets
     * gaseus spell check added to is_safe_spell check
     * added race size online editing
     * can_see check added to socket and mwhere
     + mpslay function added for triggers
     * after death trigger, mob can die in peace :)
     * offline addqp moved to 106 level
     + add SCROLL_CLANSKILL bonus item
     * OLC upgraded for ITEM_BONUS
     * double adding clan_skill only change clanskill time now.
     * prompt show added
     * fixed class_mag add in blink. :E
     * fixed olc commands show
4.71 (C) Sab
     * add gquest information for immortals and self-characters
     * command table reseen. Some security added for imm commands (NOORDER)
     * room affects structure changed. added few new parameters
     * stat room - removed room affects list. see in 'room raff'
     * raffect_update:40% for decreasing level every tick. add permanent raff.
     * toggle blink - removed useless on/off.
     * add correct addr saving
4.72 (C) Sab
     * do_blink fixed. with argment toggles blink skill. else uses social blink
     * fixed bit of spell_gaseous_form
     * added and revised new clanspells/clanskills
     * some fixes in safe_spell
     * crash with backup_restore of char with penalty fixed
     * add clanskill and quenia item_bonus to spell_ident
     * spell aid improved - nonwar and nonthi classes get GREAT aid
4.73 (C) Sab
     * quest buy <item> [train] added
     * remort - add some notifications.
     (C) Ast   : features
     + reward command for 106+
     + repair command added
     * cset limited
     * clanwork fixed
     (C) Ast   : fixes
     ! clanwork apply, addskill, remskill are now working
     ! raffect_update() fixed(!!!) - some of them were eternal!!! Sab :EEEE
     * some cosmetics (thanks, Sab)
     * do_repair fixed and improved greatly (now works ALL of parameters)
     (C) Sab
     * quest buy <item> [train] - allowed for 100+
     * little cosmetics
     * try to implement uint64 variable type
     (c)Last
     * some TRAP cosmetics and fixes
     (C) Sab
     * blacksmither moved from ee to W
     * flag STOPDOOR applying, in_doors,out_doors removed. area version changed
     * uint64 removed
     (C) Ast   : features, corrections
     ! fixes in do_repair
     * fixes in do_equipment
     + added do_damage ( damages objects- now I need it(!!!) )
     ! rewritten load_objects, load- , create- objects- defaults.
     + added item_cond_type item_cond_table[]
     * fixed spell_identify & do_lore
     * MUCH cosmetics and may be I've forgotten something :(
     (C) Ast   :(ADDON - I could forget something I've written...)
     + durability looks to be implemented...
     ! save_object & load_object corrected
     ! material_table translation and improving
     + damage_obj() & damage_both_objs() implemented 
       alpha-tested on shield_cleave
     * stat object beautified
     * much cosmetics as usual (repair, reward, damage... etc.)
     (C) Ast   : News, fixes, minimising the possibility of bugs' appearance
     ! load_objects, save_object, obj_update new checks and corrections
     ! olc OEDIT corrected
     + const char *get_obj_cond( OBJ_DATA *obj, bool gshowcon) added
     + confiscate( CD ch, const char *argument) implemented (106+, alias:seize)
     + OBJECT_ETERNAL and SHOW_COND_ETERNAL checks added
     ! damage_obj() improved due to OBJECT_ETERNAL
     ! do_lore corrected due to OBJECT_ETERNAL
     * repair informativity improved
     ! do_damage can _now really_ reach victim's inventory
     * cosmetics
     (C) Ast   : Common. Something as usually is forgotten... :(
     * ch informing string added for do_reward
     * do_blink is _THOUGH_ checked(is_exact_name(argument,"on off toggle") :E
     + ahelp material implemented
     * common bugfixings, cosmetics.
     (C) Ast   : News, fixes
     + do_devote implemented(!!!) -beta-version
     * common fixings, cosmetics
     (C) Sab
     * wander fixed
     * warnings fixed
     (C) Ast
     * Sab's BUG fixed... RRRRRRRRRR with do_delete
     + deity - imms' new command (changing deities).
     (c)Last
     + ITEM_TRAP added
     * item now needed for trap intstallation, trap type depends on item's v0
     * items available for trapping: ITEM_TRAP & ITEM_FOOD (just debug purpose)
     (c) Apc
     - damage_eq_char remmed out due to crashes (nedd to go to para - no time to fix)
     (c) Ast   : fixes in unstable parts of sources
     * clanwork <clan> <apply> <apply_modifier> <apply_value> works now
     * do_equipment riched with checks
     * do_deity and do_devote are functionalised
     + panish_effect() prototype implemented : needs upgrading,
     * do_devote temporary raised on 110 to correct new deity fild
     * wear object_eternal allowed (durability==-1)
     (c) Ast   : fix
     * do_repair       : check for ch->gold added if IS_NPC(victim)
     (c) Ast   : fixes, addings, reorganization
     * do_whois        : victim->deity string formatted
     + do_polyanarecall: command added :)
     + do_cfix         : added but uncomplete
     + carma, favour   : new pcdata fields added
     * do_deity        : smart fixes
     * do_devote       : smart fixes, improvements
     * fread_c,fwrite_c: rewritten due to new args
     + damageobj       : new global CFG added
     ~ damage_obj      : transfered and declared as global
     + do_recall(etc.) : deity is getting sence
     * do_cleave       : works again now (if config damageobj is set)
     * do_repair       : service cost changed
     * cosmetics, small corrections, forgotten fixes...:)
     (c) Ast   : bugfixes
     * do_recall       : one of checks removed
     * do_devote       : devote help deity -->> devote stat deity
     * cfix(ch)        : automatically if version needs it

4.74 (c) Apc : I'm fuckin' back
     М-да, ребята.. я за вас рад... 
     * Косметика с глобалом
     * Проглажен Астовкий devote
     * Спасибо Асту за буг с версиями, у нас иеперь ротация - после версии 22
       Все чары перешли на версию два.. :)
     - Пофиксено методом убиения старых версий (я так понял - теперб чары автоматом
       фиксятся). 
     ? Кто убрал мой код безкрашевой подгрузки чаров?? Раньше выкидывало чара, теперь
       крашит нафиг муду! :( Придется писать заново..
     * Эхх.. получите вы от меня... :)) Скоро.. кое-чего вкусненького... 
     (C) Sab
     - fixed do_snoop for lostlink players
     - little fix in do_look
     * add IS_DEITY - for test Deity godness separately from IS_ELDER check
     - add sset into command table
     * account now shows in mstat
     * added put <amount> silver into account command
     (c) Ast
     * fixes in nanny()
     + defined some applies. I'll continue editing deities by myself.
     - devote <mob> stat denied.
     * cfix a bit rewritten.
     + do_devote corrected: new parameters: pardon, excuse for Elders.
     + panish_effect() basic conception activated: 
       affects char with blind or curse for 1500 ticks but asunder out at 500.
       Time have to flow out by itself: death doesn't wipe out these spells.
       -->> fixes in: char_death, spell_remove_curse, spell_cure_blindness etc.
     + devote <char> stat if !(ch->deity) now shows brief stats: carma, etc.
     (c)Last
     * trap fixes, cosmetics
     (C) Ast
     + ch->deitynumber added: soon it will change usage of ch->deity
     + gfix for elders.
     + GTFix - new global config for temp fixes. Auto turns off after reboot.
     * char loading and saving addings and changes.
     - deity will be nilled first and ( I hope) last time :(
     * cosmetics
4.80 (C) Ast
     ! Apc: test failed... Don't do such things any more...
     + do_forge command (now for imms only)
     ~ If obj->material isn't obj->pObjIndex->material object's material
         saves separetely in char's save
     + doubled MAX_DEITY_APP constant(now 20)
     + do_gfix() new option: clearapplies(soon it will act on single targets)
     + deity <deity> apply/penalty implemented (try without parameter for info)
     + some deity applies already influences char while he's devoted
     + checks for dtab.worship in do_delete -->> decrify_worship() function
     + new ch->deity output in do_whois     : depends on ch->pcdata->favour
     + new ch->deity output in devote show  : depends on ch->pcdata->favour
     + show_deity_applies() for 'deity stat <deity>':shows deity's bonuses/pen
     + bool favour() for check&(|)update ch->pcdata->favour
     + void change_favour() for using allover the sorces
     + deity_char_power() - this function converts data from deity_table 
          and recieves bonus/penalties from it
     + new parameters in do_deity() : reward, punish ( changes favour by +50-)
     + do_supplicate() for devoted characters. 
          Parameters: recall, uprise, aid, peace, protect
     * global gtfix now can be saved in config
     - some "clever boys" tried to improve my life_stream...   : fixed
          (there were SOME check_improve()-s for evil_presence...) U R Lazy! :(
     * much improvements, commetics.
     (C) Ast
     ~ ch->deitynumber is saving in exchange of ch->deity (still being used)
     ~ ACT_BLACKSMITHER is from now ACT_FORGER
     + ROOM_FORGE flag to protect from dvarves' unequip
        repair now may be used in ROOM_FORGE only
     + do_whois output table fixed
     + CHAN_RP fixed to GAME completely at last
     + added check for pets in do_recall
     + deity output corrections
     + vote <num> restring <newtext> option added
     ~ fucking blink fixed :) ( now: noparameters, char, on, off, toggle)
     + devote stat <deity> for seeing applies ( 4 NONDEITY chars)
     * cosmetics( cases, names, acts, etc. )

     (C) Sab
     * fread_number - ignore error number strings.
     (C) Ast
     + added check for null character in music.c (was bug)
     * repair fixes
     + some favour update dependances implemented
     * ch->deitynumber is ch->pcdata->dn now
     (C) Ast
     * repair fixed ( all is left - to argue prices (2 strings in sources) )
     * supplicate fixed: more decrement favour, affect to char, waitstait
     * pray seems to be fixed... (for nonclerics increased chance to succeed)
     * mages now has 'aid' spell from fifth level
     (C) Apc
     * Добавлен новый тред - complain, для жалоб игроков друг на друга :)
     * Если уровень кроватки > уровня чара  - чар на нее не может сесть/лечь/etc
     * Если на кроватке стоит noremove - чар с нее не модет встать :)
     * no more "ты посвяшаешь statue <deity> - devote stat доступен для DEITY,
       но там стоит str_cmp :)"
     * Мелкие багфиксы, которых я просто не помню, в т.ч. твики act_move.c (см. diff)
     * Пофиксен may_devote, теперь можно девотиться :))
     (C) Sab
     * пофиксен who - убран count, так как output идет через page
     * do_offline - some fixes
     * rewritten help for do_offline
     * добавлено kins (родственники) в команду family, убран лишний аргумент
     * немного поправлен whois при выводе семейной информации
     (C) Ast
     * punish_effect fixed - now ch->pcdata->dc - updatable godcurse counter
     ! char_death fixed - now looks not to be buggy :)
     - blacksmith using on enchanter-type items disallowed :)
     * ldefend character checks added
     * ch->version raised to 5 due to some improvements (DRank,DCurse, etc.)
     - devote disavow disallowed from now
     * do_supplicate corrected in prices and waitstate
     * do_recall fixed : added check for ch->level < 21
     ~ tournament complete, cancel lowered to 105
     * cosmetics, corrections, improvements...
     (C) Ast
     * fixed find_location, etc. : NPC checks updated
     + item condition is a part of format_obj
     + item condition shortflag support added
     (C) Ast
     ~ cleave allowed to mortals
     + devote stat <deity> shows ability do devote self to this deity
     * redit fixed
     + implementing russian name using started.
     ! some bugs fixed
4.81 (C) Sab
     - fixed cool bug with tilda
     (c)Last
     - Gendalf removed from trust check in comm.c
     + added missing ';' in act_info.c
     (C) Sab
     * fixed deity loading error messages
     * fixed russian name implementing - act_parse_name bug
     * some fixes.. forget :(
     * move char to Temple from clan areas in marry area fixed
     (C) Ast
     + repair <obj> <mob> value added : valuate repairing
     ! find_location fixed in checks
     ~ set corrected, some commands level changed
     - supplicate recall is now disallowed under adrenalin and curse
     * cosmetics
     (c)Last
     + Starting bet feature for auction
     (c) Apc 
     * Addpenalty show offline char bug fixed 
     (c) Last
     * auction crash bug fixed (my fault)
     (C) Sab
     + added reboot backup2 option - copy all players file to backpu2 folder
     + added backup2 command - restores and shows files in backup2 folder
     (c)Last
     * enchant_armor effect decreased
     + players can only wear own-enchanted items
     + trap skill
     (c)Dragon
     test of cvs :)            
     * do_rename fixed  
     * string char long опустил на 105 ... по просьбе Eliot
     (c)Last
     * string char long decreased to 104
     (c) Ast
     ! rename fixed.
     + some access checks in "marry(family) room" for ROOM_ELDER
     + do_delete special allowed for CREATORs
     ! char_death : try to fix removing affects
     * cosmetical fixes, addings, corrections...
     ! recall for imms fixed...
     (c)Last
     * players can only wear own-enchanted items
     + (public) owner added to randoms(item is enchanted, but anyone can use it)
     + obj->owner is shown by lore
     * cosmetics in traps
     * dwarves guild control feature re-enabled
     * enchant_armor small corrections
     (c)Dragon
     ! fixed bug with remove_affects after death
     * fixed bug with hp/mana/move after death 
     (c) Apc
     ! two major design bugs in fight.c seem to be fixed
     (commit frow work - didn't test due to slow connection)
     1. stunnies do attack|block
     2. killing no_pk with missile wpns
     * some more fixes
     ! backup2 in now imm only!
     * some debugging in fight.c
     (c)Last
     + obj->owner now will load
     * enchant_armor formulas corrections
     * trap fixes
     * lifestream can't be casted in clans
     (c) Apc
     * Lostlink kills: now impossible if "lost" char has no adrenaline
     * Something else.. don't remember
     (c) Apc
     * auction tempfix (atoi64 is buggy)
     * stunnies should not fight at all now
     * spellaffect bug fixed (skill_lookup tweak - gigi)
     * разглажен код спеллаффекта при выводе (Aff, affstat, observation)
       и в системе - spellaffect теперь типа скилл и может нормально сейвиться в файл.
     * blacksmith multiple bless enchant fixed (если надо пофиксить и другие
       камешки - пишите)
        (c) Magic
     * file files.url renamed to files.h
     * Ubeditelnaja pros'ba, ne pishite v etom file v Win kodirovke, davajte ili
       KOI8-R ili translit. Nifiga ne vidno v terminale.
        (c) Apc
    * Curse now doesn't affect items with permanent bless, no more cont. bless stones
    * Holy Light allows seing bless/evil auras on items
    ! USE build, not version no when implementing minor changes
        (c) Sab
    + add config autopeek - autopeek when looking target option
    * test.c in KOI-8
4.81 build 1 (c) Sab
    ! APC If your want to use build - write build version in test.c
    * workaround about charge skill
    * what the MEMORY OF MOBILE in merc_def? changed A to (A), etc. but that
      variables hasn't used in sources.
4.81 b2 (c) Apc
    * Sab, tvou mat' build always increments - eto skolko raz comilili progu 
      ZA VSE VREMYA E ZHIZNI! A ne v posledney versii..
    * Suicide increments deathcounter by 3;
    * Sleeping suicide denied
    * MLT deathcounter 15 resunts PERMANENT 10hp loss - Let'em have it! }8-E
        (c) Dragon
    * fixed do_rename
    * popravlen do_backup
    * ispravleni nekotorie opechatki :)
4.82 build 491 (c) Wagner
    * From now reward in GQuest has certain level.
4.82 build 492 (c) Stinger
    * Bug with Shield Cleave fixed :)
    * Bug with Ray of Truth spell fixed.
4.82 build 493 (c) Wenom
    * Bug with suicide;sacrifice fixed:)
4.82 build 494 (c) Wagner
    * update_weather() changed a little bit.
    * added new field in "note to" - coder.     
4.82 build 495 (c) Wagner
    * do_who fixed finally    
4.83 (c) Magic What is builds? Where i can see a build number? What is the fucking abuses towards Sab?Apc, show your emotions in other place  
     Small change in gods hierarhy
     Saboteur is a ONE Implementor.
     Some commands levels raised.
     ROOM_ELDER flag changed from "elder" to "relder" (c) Dragon
     6 new rows added to test.c 
4.83.01 (c) Wagner
       2Magica: Change version is too much for this change so we use s.c.
       "build" or subversion. You can see build number by entering MUD or
       by typing "version" if you're online.
        * do_kick improved a little bit
        * do_bash received some cosmetic
        * fireshield at last make damage by itself
        * some cosmetics...
4.83.02 (c) Wagner
       * do_quest receive some cosmetic
       * from now you can't eat/drink while fighting
       * mind_channel is a cast from MIND group, oasis - from CURATIVE
4.83.03 (c) Wagner
       * bug with POS_STUNNED seems to be fixed
       * bug with aid seems to be fixed
4.83.04 (c) Wagner
       * bug with QuestMobs dying because of plague/poison finally fixed.
4.83.05 (c) Stinger
       * Bug with charged spells fixed.
       * Envenom skill upgaded.
       * Bless spell upgraded.
       * Vampires cannt cast bless spell.
       * Skeletons cannt be dirted or blinded.
       * Deities remaked.
       * Updated help for races in group.are
       * See you later, to be continued...:)
4.83.06 (c) Wagner
       * PLENTY of Udun's additions removed 
       * some cosmetic.....
       * something else, but I've already forgot.. :(((
4.83.07 (c) Wagner
       * quest convert feature restored
       * bug with negative HR in bless fixed
4.83.08 (c) Wagner
        * quest convert feature fixed
        * some cosmetics in different places...
        * dam_message "ZuZkA" is shown now if dam>1000
        * stop_follower() removes AFF_CHARM from victim->affected_by
        * account transfer feature added
4.83.09 (c) Wagner
        * Zombie/pet fixed (didn't perform orders)
        * ahelp damage has been improved a little bit
        * account get XXX <charname> feature added (for Imms only) 
                     for removing gold from <charname's> account
        * Modified assuming name for random shields
4.83.10 (c) Wagner
        * do_config now works everywhere except ROOM_ARENA.
        * If IS_CFG(victim,CFG_ZRITEL) then ch will see (ZRITEL) before victim's name.
4.83.11 (c) Wagner
        * deadly casts on self in safe room seems to be fixed
4.83.12 (c) Magic
        * mobs became weaker at low levels 
        * set initial value of weapon skill in 60% 
4.83.13 (c) Wagner
        Magica! a versiju v failike io.c xto menyat bydet? Ili eto mi poprikolu
        versii pishem?
        * bug with aid > 10 000 hp seems to be fixed
        * cosmetics and improvement in different places (for Imms only)          
        * supplicate under adrenalin is forbidden
        * bug with gas breath fixed
4.83.14 (c) Magica
        Ya 100 raz prosil skazat' gde ee smotret'
        * spell_bless apply min 4 HR for low levels (4 + level/9) 
4.83.15 (c) Wagner
        Predposlednyaya stroka faila io.c :)
        * Pri vixode aid snimaet TO4NO to koli4estvo hp, kotoroe daet.
        * note has help now.
        * Immortals will receive bugreports now
4.83.16 (c) Wagner
        * N-th attempt of "aid" fixing. 
        * note help has been improved.
4.83.17 (c) Wagner
        * some cosmetics and improvement in parse_note
        * offline delete now shows result
        * vote is now available if you have played more than 50 hours
        * newbies now recall immideately
        * gaining 5th level newbies will receive rather RP message
        * some cosmetics in different places...
4.83.18 (c) Ghost
        * charge fixed
        * & cvs test :)
4.83.19 (c) Wagner
        * quest counter in remort fixed
        * deity pri podnyatii s backup'a ne obnulyaetsya. Nax obdylyat bilo? special bugusing?
4.83.20 (c) Ghost
        Wagner, "some cosmetics in different places" - это как и где? :)
        * нападение с offencive spell без дамаги (curse, poison) на старших чаров не в PK, fixed
        * statues: 
          телепортятся, видимы только иммами и девотнутыми этому божеству, не видны по where и scan
          при pray девотнутого возле статуи - restore и +favour
          Блин, муда постоянно в состоянии глобал квеста.. мечта идиота :))
          ЗЫ. А кто будет лоадить статуи - давать по рукам. Больно.
          Если она уже так нужна (посмотреть там, потестить чего-нить), то трансферьте.
          Потому что если их наделать десятка два каждой, они полный беспредел устроят :).
          ЗЗЫ. Да, приписаны они к замку богов
        * 1 favour снимается каждый тик с вероятностью 1%
        * при отрицательном favour хуже работают скиллы, вплоть до 50% :). 
          Ну это сильно постараться надо.
        * в multi_hit проверка на шанс прохождения уникального спелла, 0-25%, вызывается функция
          tatoo_works. Была мысля сделать 2 number_percent, чтобы дробные шансы тоже считались
          но потом забил :) Смысла не вижу, это не сейвсы.
        * tatoo_works - собственно, она самая. Позже надо будет проставить даммессаги, 
          А то там сейчас просто "Ты царапаешь <victim>", etc. Криво, некрасиво, но работает :).
        * слегка поправлен информ от deities, to be continued..
        * Убрано (закомментировано тремя ///) куча вызовов change_favour в спеллах и скиллах
          в magic.c и fight.c. Я всегда говорил, что за спеллы favour давать нельзя.
          Но никто не верил.
        * В начале do_train добавлен вейтстейт. Так, на всякий случай :).
4.83.21 (c) Wagner
        Ghost, "some cosmetics in different places" - это может быть от 
        банальной очепятки в коде, изменения названия, другой/просто раскраски 
        до добавления чего-то (типа забытая строка хелпа в каком-то месте :) )
        Конкретно сегодня одно из - offline stat у ньюбисов криво показывалось 
        купе (2 пробела не хватало :) ) 
        * bug with plague seems to be fixed
4.83.22 (c) Ghost
        В one_hit две проверки на fireshield. Чье? :) Пока не трогал, может это фича.
        * Статуям нельзя ничего давать (а то тут уже народ про морф заикается :))
        * energy drein, double aid, vampiric & vamp_mana fixed.
          Теперь хп и маны больше максимума не будет. Ну, я надеюсь.
        * Слегка пофиксил пару богов в tatoo_works. 
          А то некоторые жирненькие такие получились :)
        * физдамаг по чарам теперь зависит от кона жертвы
4.83.23 (c) Wagner
        * spell oasis no longer belongs to regeneration group (c) Wagner 
          (kakogo x'ya on tam delal (c) Ghost)
        * some cosmetics in different places 
          (2Ghost: * покрашено в {y цвет имя появляющегося и уходящего в МУДе
                   * перекрашен пталк в {W цвет и question в {y цвет
                   * покрашен enchant armor/weapon 
                   * пофиксил пару опечаток в коде
                   * еще чего-то в том же духе, но уже не помню где.)
        * теперь низзя одним гномом на всю братву шмотки манчкинить
          obj->owner жестоко не даст получить вожделенный ништяк.
4.83.24 (c) Wagner
        * crush bug with auction seems to be fixed
        * crush bug with "stat mob" seems to be fixed
        * some cosmetics in different places as a rule :)
4.83.25 (c) Saboteur
        * some crushes with auction, tatoo works (deities table) and other
        * added 'used' parameter to do_mem
        * removed unused GUILD_DATA
        * added MAX_DEITIES define, rewritten some code with using max_deities.
        * fixed potential crashing bug with add/remove deities..
        * improved do_mem to show near to real mem used for structures
4.83.26 (c) Wagner
        * wizhelp стал чуток понятнее (надеюсь)
        * пару косметических исправлений
4.83.27 (c) Ghost
        Есть замечательная идея :).
        Включить new_one_hit, а старую нафиг. В ней все равно не работают эти thac0, thac32.
        Вот только там подправить надо немного сначала.
        * fortitude bug fixed. Проверялось fortitude чара, а не жертвы. 
          Причем без проверки, магический дамаг или нет.
        * дамаг от bash'а повешен на левел.
        * Добавлен define IS_STATUE, соответственно переписано пару 
          "маразматических участков кода" :)
        * Увеличено влияние (ch->int-victim->wis) на дамаг магией
        * Во всех бризах строчка типа
          dam += URANGE(-dam/8,dam*get_int_modifier(..)/200,dam/8);
          заменена на
          dam += dam*get_int_modifier(..)/200;
          Наикса вообще там URANGE был? Того, что в get_int_modifier вполне хватает.
          Тем более, что они разные. Надо вообще-то все дамажливые спеллы перешерстить,
          поуменьшать дайсы и не делать зарезания, если дамаг магический. 
          Никто не хочет заняться? :)
          Итого: Бризовый дамаг теперь в зависимости от инта колеблется от dam*3/4 до dam*2,
          Остальная магия - от dam/2 до dam*3
        * Поменял циферки в get_ac_modifier, увеличив нижний порог влияния АС на дамаг.
          Примерно на 10%, так как по моим подсчетам бризы уступают физикалу  
          как раз процентов на 10, уже с подправленным get_int_modifier
        * Каждый бриз теперь висит на одной, своей родной стихие
        * Увеличен favour, дающийся при поклонении
        * Слегка пофиксил пару богов в tatoo_works. Традиция, однако :).
4.83.28 (c) Ghost
        * Боги помогают только девотнутым, а не тем, у кого favour>0, как было раньше.
        * За глобалы дают немного favourа
4.83.29 (c) Wagner
        * complains подключены (сделать Арс их сделал, а подключить забыл)
        * gas breath теперь висит на Air.
        * Пришел конец сидению на 25 левеле в армии. 
          Теперь при получении 22 левела "майор Молчанов" будет выводить из армии.
        * Подправлен mstat_info/do_mstat  
4.83.30 (C) Sab
        * попытка поправить варнинг в do_wizhelp
        * вернул назад make.bat 
          (для некоторых - есть хорошая команда make clean,
          и эта пауза с запуском муда мешает.. :)
4.83.31 (c) Wagner
        * по просьбам трудящихся wizhelp показывает все комманды доступные на 
          конкретном левеле
4.83.32 (c) Ghost
        * new spell mind blast. For casters against casters :).
        * chance to backstab mobs depends on level. 
          Base chance to hit was near 15% for human. Now I think it's near 65%
        * traditionally, some fixes in tatoo_works :)
        * acid blast improved and moved to 40, fireball improved and moved to 28
4.83.33 (c) Wagner
        * complains теперь ходят только к иммам, а не ко всем
        * mind blast теперь иммам доступен :)))
4.83.34 (c) Ghost
        * Пофиксен буг с отрицательным дамагом
          Кто Ибрагиму damage 1d1 делал? Он стабы отрицательные выдает (ну, выдавал) :)
          Причем это уже давно
        * PC группы из чаров в PK range теперь получают бонус на экспу
          Надо же когда-то начинать...
        * Слегка поправлена доступность mind blast'a
4.83.35 (c) Ghost
        * Поправлены некоторые циферки, в частности уменьшен favour от статуй, 
          увеличено влияние информа на дамаг магией
        * Исправлена пара очепяток где-то в spell_observation и окрестностях :)
        * Deity теперь не обнуляется и при backup2 restore 
        * Усилены проверки для бонуса на экспу
4.83.36 (c) Ghost
        * неудачное поднятие слишком большого зомба дает чару несколько вейтстейтов
4.83.37 (c) Wagner
        * Рандомные щиты теперь ресетятся в инвентори, а не одеваются по ХХХ штук
          на мобов.
        * Добавлены мессаджи повреждений при помощи deity в tatoo_works. 
          (ох и криво у нас написаны они....)
        * Поправлена проверка на питье-еду во время боя.
        * unread catchup - и все нечитанное прочитано :)
4.83.37a (c) Wagner
        * unread catchup fixed
4.83.38 (c) Ghost
        * При flee на wimpy есть вероятность, что wimpy сбросится в 0. 
          Выставление wimpy - waitstate.
        * Небольшие правки в may_devote
        * При убийстве на арене одноверца favour не падает
        * clanwork create fixed
4.83.39 (c) Wagner
        * know person updated. Теперь кланфитера видно.
        * outfit fixed. Детям стали в школе щит выдавать, а не только оружие.
        * Пару очепяток пофиксено.
4.83.40 (c) Ghost
        * Как обычно, поправлено пару строчек в tatoo_works :)
        * Уменьшен физдамаг, прошерстена пара скиллов на предмет дисбаланса
        * Recall агров теперь невозможен
        * Шмотки, кованые блессом, не курсаются в принципе
        * Вроде пофиксен баг Шико с зомбами
4.83.41 (c) Ghost
        * Изменены цифры в do_cleave 
        * Привинчена клановая спеллка Shine of Heaven (Стражи Небес)
        * Create water fixed
        * Клановые спеллы теперь не чаржатся
        * Добавлен define IS_GUARD(ch)
4.83.42 (c) Ghost
        * enchant armor стал более реален не для дварфов. Ну у дварфов старые плюшки.
        * поскольку бугу с аффектами найти пока не удалось, привинтил heal release - 
          снимает все ch->affected и ch->affected_by
4.83.43 (c) Ghost
        * re-enchant fixed
        * shine of heaven updated
        * fixed crash when buying enchanted weapon
        * Буг со смертью от голода и записью этого в журнал, fixed
4.83.44 (c) Ghost
        * буга с heal release fixed
4.83.45 (C) Sab
        * fixed warnings with integer size under FreeBSD 5.2.1
        * do_equipment disabled - waiting for fixing crashing bug
4.83.46 (C) Saboteur
	* Fixed critical bug with description. ~ character was not masked.
