/**
 * @file
 * @brief Misc function related to player transformations.
**/

#ifndef TRANSFOR_H
#define TRANSFOR_H

#include <set>

#include "enum.h"
#include "player.h"

#define HYDRA_FORM_HEADS_KEY "hydra_form_heads"
#define MAX_HYDRA_HEADS 20

enum form_capability
{
    FC_DEFAULT,
    FC_ENABLE,
    FC_FORBID
};

class FormAttackVerbs
{
public:
    FormAttackVerbs(const char *_weak, const char *_medium,
                    const char *_strong, const char *_devastating) :
    weak(_weak), medium(_medium), strong(_strong), devastating(_devastating)
    { };

public:
    const char * const weak;
    const char * const medium;
    const char * const strong;
    const char * const devastating;
};

enum duration_power_scaling
{
    PS_NONE,                // no bonus
    PS_TENTH,               // bonus based on rand2(power)/10
    PS_SINGLE,              // bonus based on rand2(power)
    PS_ONE_AND_A_HALF,      // bonus based on r(power) + r(power/2)
    PS_DOUBLE               // bonus based on r(power) + r(power)
};

class FormDuration
{
public:
    FormDuration(int _base, duration_power_scaling _scaling_type, int _max) :
    base(_base), scaling_type(_scaling_type), max(_max) { };

    int power_bonus(int pow) const;

public:
    // base duration (in 10*aut, probably)
    const int base;
    // the extent to which spellpower affects duration scaling
    const duration_power_scaling scaling_type;
    // max duration (in 10*aut, probably)
    const int max;
};

class Form
{
public:
    Form(string _short_name, string _long_name, string _wiz_name,
         string _description,
         int _blocked_slots,
         int _resists,
         FormDuration _duration,
         int _str_mod, int _dex_mod,
         size_type _size, int _hp_mod, int _stealth_mod,
         int _flat_ac, int _power_ac, int _xl_ac,
         bool _can_cast, int _spellcasting_penalty,
         int _unarmed_hit_bonus, int _base_unarmed_damage,
         brand_type _uc_brand, int _uc_colour,
         string _uc_attack,
         FormAttackVerbs _uc_attack_verbs,
         form_capability _can_fly, form_capability _can_swim,
         form_capability _can_bleed, bool _breathes, bool _keeps_mutations,
         string _shout_verb, int _shout_volume_modifier,
         string _hand_name, string _foot_name,
         monster_type _equivalent_mons) :
    short_name(_short_name), wiz_name(_wiz_name),
    duration(_duration),
    str_mod(_str_mod), dex_mod(_dex_mod),
    blocked_slots(_blocked_slots), size(_size), hp_mod(_hp_mod),
    can_cast(_can_cast), spellcasting_penalty(_spellcasting_penalty),
    unarmed_hit_bonus(_unarmed_hit_bonus), uc_colour(_uc_colour),
    uc_attack_verbs(_uc_attack_verbs),
    can_bleed(_can_bleed), breathes(_breathes),
    keeps_mutations(_keeps_mutations),
    shout_verb(_shout_verb), shout_volume_modifier(_shout_volume_modifier),
    hand_name(_hand_name), foot_name(_foot_name),
    long_name(_long_name), description(_description),
    resists(_resists), stealth_mod(_stealth_mod),
    base_unarmed_damage(_base_unarmed_damage),
    can_fly(_can_fly), can_swim(_can_swim),
    flat_ac(_flat_ac), power_ac(_power_ac), xl_ac(_xl_ac),
    uc_brand(_uc_brand), uc_attack(_uc_attack),
    equivalent_mons(_equivalent_mons)
    { };

    bool slot_available(int slot) const;
    virtual bool can_wear_item(const item_def& item) const;

    int get_duration(int pow) const;

    virtual monster_type get_equivalent_mons() const { return equivalent_mons; }

    virtual string get_long_name() const { return long_name; }
    virtual string get_description(bool past_tense = false) const;
    virtual string transform_message(transformation_type previous_trans) const;
    virtual string get_transform_description() const { return description; }
    virtual string get_untransform_message() const;

    virtual int res_fire() const;
    virtual int res_cold() const;
    int res_neg() const;
    bool res_elec() const;
    int res_pois() const;
    bool res_rot() const;
    bool res_acid() const;
    bool res_sticky_flame() const;

    virtual int get_stealth_mod() const { return stealth_mod; }
    virtual int get_ac_bonus() const;
    virtual int get_base_unarmed_damage() const { return base_unarmed_damage; }
    virtual brand_type get_uc_brand() const { return uc_brand; }
    virtual string get_uc_attack_name(string default_name) const;

    bool enables_flight() const;
    bool forbids_flight() const;
    bool forbids_swimming() const;

    bool player_can_fly() const;
    bool player_can_swim() const;

public:
    // Status light ("Foo"); "" for none
    const string short_name;
    // "foo"; used for wizmode transformation dialogue
    const string wiz_name;

    // A struct representing the duration of the form, based on power etc
    const FormDuration duration;

    // flat stat bonuses
    const int str_mod;
    const int dex_mod;

    // a bitfield representing a union of (1 << equipment_type) values for
    // equipment types unusable in this form
    const int blocked_slots;
    // size of the form
    const size_type size;
    // 10 * multiplier to hp/mhp (that is, 10 is base, 15 is 1.5x, etc)
    const int hp_mod;

    // can the player cast while in this form?
    const bool can_cast;
    // increase to spell fail rate (value is weird - see spell_fail())
    const int spellcasting_penalty;

    // acc bonus when using UC in form
    const int unarmed_hit_bonus;
    // colour of 'weapon' in UI
    const int uc_colour;
    // a set of verbs to use based on damage done, when using UC in this form
    const FormAttackVerbs uc_attack_verbs;

    // has blood (used for sublimation and bloodsplatters)
    const form_capability can_bleed;
    // see player::is_unbreathing
    const bool breathes;
    // "Used to mark forms which keep most form-based mutations."
    // ugh
    const bool keeps_mutations;

    // what verb does the player use when shouting in this form?
    const string shout_verb;
    // a flat bonus (or penalty) to shout volume
    const int shout_volume_modifier;

    // names of the player's appendages when in the form; "" if form doesn't
    // change them (fallback to species, etc)
    const string hand_name;
    const string foot_name;

protected:
    // "foo-form" &c; used for morgue and % screen. "" for none
    const string long_name;
    // "a barish foo." used for @ screen and transform messages. "" for none
    const string description;

    // bitfield representing a union of mon_resist_flags for resists granted
    // by the form
    const int resists;

    // a multiplier to Stealth skill for player stealth calculations.
    // if 0, fall back to species values.
    const int stealth_mod;

    // base unarmed damage provided by the form. (note that BUD is not
    // equivalent to weapon damage...)
    const int base_unarmed_damage;

private:
    bool all_blocked(int slotflags) const;

private:
    // whether the form enables, forbids, or does nothing to the player's
    // ability to fly / swim (traverse deep water).
    const form_capability can_fly;
    const form_capability can_swim;

    // flat bonus to player AC when in the form.
    const int flat_ac;
    // spellpower-based bonus to player AC; multiplied by power / 100
    const int power_ac;
    // experience level-based bonus to player AC; XL * xl_ac / 100
    const int xl_ac;

    // the brand of the attack (SPWPN_FREEZING, etc)
    const brand_type uc_brand;
    // the name of the uc 'weapon' in the HUD (e.g 'ice fists')
    // "" to use default
    const string uc_attack;

    // a monster corresponding to the form; used for console player glyphs
    const monster_type equivalent_mons;
};
const Form* get_form(transformation_type form = you.form);

bool form_can_wield(transformation_type form = you.form);
bool form_can_wear(transformation_type form = you.form);
bool form_can_fly(transformation_type form = you.form);
bool form_can_swim(transformation_type form = you.form);
bool form_likes_water(transformation_type form = you.form);
bool form_likes_lava(transformation_type form = you.form);
bool form_changed_physiology(transformation_type form = you.form);
bool form_can_bleed(transformation_type form = you.form);
bool form_can_use_wand(transformation_type form = you.form);
bool form_can_wear_item(const item_def& item,
                        transformation_type form = you.form);
// Does the form keep the benefits of resistance, scale, and aux mutations?
bool form_keeps_mutations(transformation_type form = you.form);

bool feat_dangerous_for_form(transformation_type which_trans,
                             dungeon_feature_type feat);

bool check_form_stat_safety(transformation_type new_form);

bool transform(int pow, transformation_type which_trans,
               bool involuntary = false, bool just_check = false);

// skip_move: don't make player re-enter current cell
void untransform(bool skip_move = false);

size_type transform_size(int psize = PSIZE_BODY,
                         transformation_type form = you.form);

void remove_one_equip(equipment_type eq, bool meld = true,
                      bool mutation = false);
void unmeld_one_equip(equipment_type eq);

monster_type transform_mons();
string blade_parts(bool terse = false);
monster_type dragon_form_dragon_type();
int hydra_form_heads();
void set_hydra_form_heads(int heads);
const char* transform_name(transformation_type form = you.form);

int form_hp_mod();

void emergency_untransform();
void merfolk_start_swimming(bool step = false);
void merfolk_stop_swimming();

#endif
