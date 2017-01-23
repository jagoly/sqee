#pragma once

#include "../systems/WorldStuff.hpp"

#include "editors.hpp"
#include "other.hpp"

//============================================================================//

namespace sqt { namespace api {

//============================================================================//

using Int32 = std::int32_t;
using StringVec = std::vector<std::string>;
using Stuff = sqt::sys::WorldStuff;

//============================================================================//

Int32 create_root_entity(Stuff& stuff, const StringVec& components);

Int32 create_entity(Stuff& stuff, Int32 parent, const StringVec& components);

//============================================================================//

AnimationEdit begin_animation(Stuff& stuff, Int32 id, const AnimationTimeline& timeline);

//============================================================================//

SoundEdit play_sound_global(Stuff& stuff, const string& path, uint group);

SoundEdit play_sound_position(Stuff& stuff, const string& path, uint group, Vec3F position);

SoundEdit play_sound_entity(Stuff& stuff, const string& path, uint group, Int32 entity);

void set_sound_group_volume(Stuff& stuff, uint group, float volume);

//============================================================================//

}} // namespace sqt::api
