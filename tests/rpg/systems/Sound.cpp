#include <sqee/debug/Logging.hpp>
#include <sqee/sound/Sound.hpp>

#include "../components/Transform.hpp"

#include "../resources/Manager.hpp"

#include "Sound.hpp"

using namespace sqt;
namespace maths = sq::maths;

SoundSystem::SoundSystem()
{
}

void SoundSystem::impl_play(const string& path, uint8_t group)
{
    SoundData data {sq::Sound(), group, nullptr, 100.f};
    data.sound.set_volume(100.f * mGroupVolumes[group]);
    data.sound.set_wave(acquire_SoundWave(path));

    dop::insert(mSoundTable, std::move(data));
    mSoundTable.data.back().sound.play();
}


SoundSystem::SoundID SoundSystem::play(const string& path, uint8_t group)
{
    sq::log_info("playing global sound %s", path);

    impl_play(path, group);

    mSoundTable.data.back().sound.set_relative(true);

    return mSoundTable.counter.previous;
}

SoundSystem::SoundID SoundSystem::play(const string& path, uint8_t group, Vec3F position)
{
    sq::log_info("playing local sound %s", path);

    impl_play(path, group);

    mSoundTable.data.back().sound.set_relative(false);
    mSoundTable.data.back().sound.set_position(position);

    return mSoundTable.counter.previous;
}

SoundSystem::SoundID SoundSystem::play(const string& path, uint8_t group, EntityID entity)
{
    sq::log_info("playing entity sound %s", path);

    impl_play(path, group);

    mSoundTable.data.back().sound.set_relative(false);
    auto position = entity->get_component<TransformComponent>()->matrix[3];
    mSoundTable.data.back().sound.set_position(Vec3F(position));

    return mSoundTable.counter.previous;
}

void SoundSystem::set_sound_volume(SoundID id, float volume)
{
    SoundData& item = dop::get(mSoundTable, id);
    item.volume = volume * mGroupVolumes[item.group];
    item.sound.set_volume(item.volume);
}

void SoundSystem::set_sound_loop(SoundID id, bool loop)
{
    dop::get(mSoundTable, id).sound.set_loop(loop);
}

void SoundSystem::set_group_volume(uchar group, float volume)
{
    mGroupVolumes[group] = volume;

    for (auto& item : mSoundTable.data)
        if (item.group == group)
            item.sound.set_volume(item.volume * volume);
}

void SoundSystem::system_refresh()
{
    auto predicate = [](const auto& s) { return s.sound.check_stopped(); };
    sq::algo::multi_erase_if(mSoundTable.data, mSoundTable.ids, predicate);

    for (auto& item : mSoundTable.data)
    {
        if (item.entity != nullptr)
        {
            // still using old transform system
            auto transform = item.entity->get_component<TransformComponent>();
            item.sound.set_position(Vec3F(transform->matrix[3]));
        }
    }
}
