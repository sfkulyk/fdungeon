// $Id: magic.h,v 1.17 2004/07/29 06:39:13 mud Exp $
// Copyrights (C) 1998-2001, Forgotten Dungeon team.
// Read ours copyrights and license terms in 'license.fd'
// Spell functions. Defined in magic.c and magic2.c
DECLARE_SPELL_FUN( spell_null              ); 
DECLARE_SPELL_FUN( spell_acid_blast        ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_armor             ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_bless             ); // standart
DECLARE_SPELL_FUN( spell_blindness         ); // standart
DECLARE_SPELL_FUN( spell_burning_hands     ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_call_lightning    ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_calm              ); // standart
DECLARE_SPELL_FUN( spell_cancellation      ); // standart
DECLARE_SPELL_FUN( spell_cause_critical    ); // standart
DECLARE_SPELL_FUN( spell_cause_light       ); // standart
DECLARE_SPELL_FUN( spell_cause_serious     ); // standart
DECLARE_SPELL_FUN( spell_change_sex        ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_chain_lightning   ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_charm_person      ); // standart
DECLARE_SPELL_FUN( spell_chill_touch       ); // standart
DECLARE_SPELL_FUN( spell_colour_spray      ); // standart
DECLARE_SPELL_FUN( spell_continual_light   ); // Changed by Adron
DECLARE_SPELL_FUN( spell_control_weather   ); // standart
DECLARE_SPELL_FUN( spell_create_food       ); // standart
DECLARE_SPELL_FUN( spell_create_rose       ); // standart
DECLARE_SPELL_FUN( spell_create_spring     ); // standart
DECLARE_SPELL_FUN( spell_create_water      ); // standart
DECLARE_SPELL_FUN( spell_cure_blindness    ); // standart
DECLARE_SPELL_FUN( spell_cure_critical     ); // improved bySaboteur
DECLARE_SPELL_FUN( spell_cure_disease      ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_cure_light        ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_cure_poison       ); // standart
DECLARE_SPELL_FUN( spell_cure_serious      ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_curse             ); // improved by Adron/Saboteur
DECLARE_SPELL_FUN( spell_demonfire         ); // standart
DECLARE_SPELL_FUN( spell_detect_evil       ); // standart
DECLARE_SPELL_FUN( spell_detect_good       ); // standart
DECLARE_SPELL_FUN( spell_detect_hidden     ); // standart
DECLARE_SPELL_FUN( spell_detect_invis      ); // standart
DECLARE_SPELL_FUN( spell_detect_magic      ); // rewritten by Saboteur
DECLARE_SPELL_FUN( spell_detect_poison     ); // standart
DECLARE_SPELL_FUN( spell_dispel_evil       ); // standart
DECLARE_SPELL_FUN( spell_dispel_good       ); // standart
DECLARE_SPELL_FUN( spell_dispel_magic      ); // standart
DECLARE_SPELL_FUN( spell_earthquake        ); // standart
DECLARE_SPELL_FUN( spell_enchant_armor     ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_enchant_weapon    ); // standart
DECLARE_SPELL_FUN( spell_energy_drain      ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_faerie_fire       ); // standart
DECLARE_SPELL_FUN( spell_faerie_fog        ); // standart
DECLARE_SPELL_FUN( spell_farsight          ); // (C) Saboteur
DECLARE_SPELL_FUN( spell_fireball          ); // improved by Ghost
DECLARE_SPELL_FUN( spell_fireproof         ); // standart
DECLARE_SPELL_FUN( spell_flamestrike       ); // standart
DECLARE_SPELL_FUN( spell_floating_disc     ); // standart
DECLARE_SPELL_FUN( spell_fly               ); // standart
DECLARE_SPELL_FUN( spell_frenzy            ); // standart
DECLARE_SPELL_FUN( spell_gate              ); // standart
DECLARE_SPELL_FUN( spell_giant_strength    ); // changed by Saboteur
DECLARE_SPELL_FUN( spell_harm              ); // standart
DECLARE_SPELL_FUN( spell_haste             ); // standart
DECLARE_SPELL_FUN( spell_heal              ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_heat_metal        ); // standart
DECLARE_SPELL_FUN( spell_holy_word         ); // standart
DECLARE_SPELL_FUN( spell_identify          ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_infravision       ); // standart
DECLARE_SPELL_FUN( spell_invis             ); // standart
DECLARE_SPELL_FUN( spell_know_person       ); // (C) Saboteur, (idea by Dika)
DECLARE_SPELL_FUN( spell_lightning_bolt    ); // standart
DECLARE_SPELL_FUN( spell_locate_object     ); // standart
DECLARE_SPELL_FUN( spell_magic_missile     ); // changed by 
DECLARE_SPELL_FUN( spell_mass_healing      ); // standart
DECLARE_SPELL_FUN( spell_mass_invis        ); // standart
DECLARE_SPELL_FUN( spell_nexus             ); // standart
DECLARE_SPELL_FUN( spell_pass_door         ); // standart
DECLARE_SPELL_FUN( spell_plague            ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_poison            ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_portal            ); // standart
DECLARE_SPELL_FUN( spell_protection_evil   ); // standart
DECLARE_SPELL_FUN( spell_protection_good   ); // standart
DECLARE_SPELL_FUN( spell_ray_of_truth      ); // standart
DECLARE_SPELL_FUN( spell_recharge          ); // standart
DECLARE_SPELL_FUN( spell_refresh           ); // standart
DECLARE_SPELL_FUN( spell_remove_curse      ); // changed by Saboteur
DECLARE_SPELL_FUN( spell_sanctuary         ); // standart
DECLARE_SPELL_FUN( spell_shocking_grasp    ); // standart
DECLARE_SPELL_FUN( spell_shield            ); // rewritten by saboteur
DECLARE_SPELL_FUN( spell_sleep             ); // standart
DECLARE_SPELL_FUN( spell_slow              ); // changed by Saboteur
DECLARE_SPELL_FUN( spell_ensnare           ); // (C) Imperror
DECLARE_SPELL_FUN( spell_stone_skin        ); // improved by Saboteur
DECLARE_SPELL_FUN( spell_summon            ); // standart
DECLARE_SPELL_FUN( spell_teleport          ); // standart
DECLARE_SPELL_FUN( spell_ventriloquate     ); // changed by Saboteur
DECLARE_SPELL_FUN( spell_weaken            ); // changed
DECLARE_SPELL_FUN( spell_word_of_recall    ); // standart
DECLARE_SPELL_FUN( spell_acid_breath       ); // standart
DECLARE_SPELL_FUN( spell_fire_breath       ); // standart
DECLARE_SPELL_FUN( spell_frost_breath      ); // standart
DECLARE_SPELL_FUN( spell_gas_breath        ); // standart
DECLARE_SPELL_FUN( spell_lightning_breath  ); // standart
DECLARE_SPELL_FUN( spell_general_purpose   ); // standart
DECLARE_SPELL_FUN( spell_high_explosive    ); // standart
DECLARE_SPELL_FUN( spell_resurrect         ); // (C) Saboteur
DECLARE_SPELL_FUN( spell_aid               ); // (C) Cortney, fixed by Adron&Saboteur
DECLARE_SPELL_FUN( spell_regeneration      ); // (C) Saboteur
DECLARE_SPELL_FUN( spell_sunray            ); // (C) Jasana
DECLARE_SPELL_FUN( spell_observation       ); // (C) Jasana
DECLARE_SPELL_FUN( spell_fireshield        ); // (C) Saboteur
DECLARE_SPELL_FUN( spell_spirit_lash       ); // (C) Jasana
DECLARE_SPELL_FUN( spell_bcloud            ); // (C) Saboteur
DECLARE_SPELL_FUN( spell_distortion        ); // (C) Saboteur
DECLARE_SPELL_FUN( spell_magic_web         ); // (C) Saboteur
DECLARE_SPELL_FUN( spell_wbreath           ); // (C) Gendalf
DECLARE_SPELL_FUN( spell_oasis             ); // (C) Saboteur
DECLARE_SPELL_FUN( spell_mind_ch           ); // (C) Saboteur
DECLARE_SPELL_FUN( spell_evil_presence     ); // (C) Astellar
DECLARE_SPELL_FUN( spell_life_stream       ); // (C) Astellar
DECLARE_SPELL_FUN( spell_gaseous_form      ); // (C) Astellar

DECLARE_SPELL_FUN( spell_mummy             ); // (C)Saboteur
DECLARE_SPELL_FUN( spell_safty_place       ); // (C)Udun
DECLARE_SPELL_FUN( spell_iron_skin         ); // (C)Udun
DECLARE_SPELL_FUN( spell_inspire           ); // (C)Udun
DECLARE_SPELL_FUN( spell_rejuvinate        ); // (C)Udun
DECLARE_SPELL_FUN( spell_peace             ); // (C)Udun
DECLARE_SPELL_FUN( spell_violence          ); // (C)Udun
DECLARE_SPELL_FUN( spell_spook             ); // (C)Udun
DECLARE_SPELL_FUN( spell_madness           ); // (C)Udun
DECLARE_SPELL_FUN( spell_breath_death      ); // (C)Udun

DECLARE_SPELL_FUN( spell_eyes_death        ); // (C)Udun
DECLARE_SPELL_FUN( spell_pain	           ); // (C)Udun
DECLARE_SPELL_FUN( spell_cursed_lands      ); // (C)Udun
DECLARE_SPELL_FUN( spell_nightfall         ); // (C)Udun
DECLARE_SPELL_FUN( spell_vision            ); // (C)Udun
DECLARE_SPELL_FUN( spell_dark_swarm        ); // (C)Udun
DECLARE_SPELL_FUN( spell_power_dark        ); // (C)Udun
DECLARE_SPELL_FUN( spell_daylight          ); // (C)Udun
DECLARE_SPELL_FUN( spell_consecrate        ); // (C)Udun
DECLARE_SPELL_FUN( spell_nimbus            ); // (C)Udun
DECLARE_SPELL_FUN( spell_turn              ); // (C)Udun
DECLARE_SPELL_FUN( spell_shine             ); // (C)Udun
DECLARE_SPELL_FUN( spell_blast             ); // (C)Udun
DECLARE_SPELL_FUN( spell_magic_barrier     ); // (C)Udun
DECLARE_SPELL_FUN( spell_time_stop         ); // (C)Udun
DECLARE_SPELL_FUN( spell_incineration      ); // (C)Udun

DECLARE_SPELL_FUN( spell_mind_blast        ); // (c)Ghost

DECLARE_SPELL_FUN( spell_shine_of_heaven   ); // (c)Ghost

