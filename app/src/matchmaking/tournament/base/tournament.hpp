#pragma once

#include <vector>

#include <affinity/affinity_manager.hpp>
#include <book/opening_book.hpp>
#include <engine/uci_engine.hpp>
#include <globals/globals.hpp>
#include <matchmaking/output/output.hpp>
#include <matchmaking/scoreboard.hpp>
#include <matchmaking/timeout_tracker.hpp>
#include <matchmaking/tournament/roundrobin/match_generator.hpp>
#include <types/tournament.hpp>
#include <util/cache.hpp>
#include <util/file_writer.hpp>
#include <util/game_pair.hpp>
#include <util/logger/logger.hpp>
#include <util/threadpool.hpp>

namespace fastchess {

class BaseTournament {
    using EngineCache = util::CachePool<engine::UciEngine, std::string>;

   public:
    BaseTournament(const stats_map &results);
    virtual ~BaseTournament();

    // Starts the tournament
    virtual void start();

    [[nodiscard]] stats_map getResults() noexcept { return scoreboard_.getResults(); }

   protected:
    using start_callback    = std::function<void()>;
    using finished_callback = std::function<void(const Stats &stats, const std::string &reason, const engines &)>;

    // Gets called after a game has finished
    virtual void startNext() = 0;

    // creates the matches
    virtual void create() = 0;

    // Function to save the config file
    void saveJson();

    // play one game and write it to the pgn file
    void playGame(const GamePair<EngineConfiguration, EngineConfiguration> &configs, start_callback start,
                  finished_callback finish, const book::Opening &opening, std::size_t round_id, std::size_t game_id);

    // We keep engines alive after they were used to avoid the overhead of starting them again.
    EngineCache engine_cache_ = {};
    util::ThreadPool pool_    = util::ThreadPool(1);

    ScoreBoard scoreboard_          = ScoreBoard();
    TimeoutTracker timeout_tracker_ = TimeoutTracker();

    std::unique_ptr<MatchGenerator> generator_;
    std::unique_ptr<IOutput> output_;
    std::unique_ptr<affinity::AffinityManager> cores_;
    std::unique_ptr<util::FileWriter> file_writer_pgn_;
    std::unique_ptr<util::FileWriter> file_writer_epd_;
    std::unique_ptr<book::OpeningBook> book_;

    // number of games played
    std::atomic<std::uint64_t> match_count_ = 0;
    std::uint64_t initial_matchcount_;

   private:
    [[nodiscard]] std::size_t setResults(const stats_map &results);

    [[nodiscard]] int getMaxAffinity(const std::vector<EngineConfiguration> &configs) const noexcept;
};

}  // namespace fastchess
