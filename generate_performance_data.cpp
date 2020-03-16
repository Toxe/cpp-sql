#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include "date.h"
#include "tz.h"

struct Generator {
    Generator(const std::vector<std::string>& postfixes, std::mt19937& gen, int min_duratiton, int max_duration)
        : postfixes_{postfixes},
          gen_{gen},
          dist_postfix_{std::uniform_int_distribution<std::size_t>(0, postfixes.size() - 1)},
          dist_duration_{std::uniform_int_distribution<int>(min_duratiton, max_duration)}
    {
    }

    const std::string postfix() { return postfixes_[dist_postfix_(gen_)]; }
    int duration() { return dist_duration_(gen_); }

private:
    const std::vector<std::string>& postfixes_;
    std::mt19937& gen_;
    std::uniform_int_distribution<std::size_t> dist_postfix_;
    std::uniform_int_distribution<int> dist_duration_;
};

int main()
{
    const std::vector<std::string> prefixes{"doc", "project", "user", "collection", "file"};
    const std::vector<std::string> verbs{"create", "delete", "update", "move", "download", "archive"};
    const std::vector<std::string> dialogs{"dialog_create", "dialog_delete", "dialog_show", "dialog_move", "dialog_download"};
    const std::vector<std::string> content{"view", "view_all", "view_archive"};

    std::random_device rd;
    std::mt19937 gen(rd());

    std::discrete_distribution<std::size_t> dist_source({60, 10, 30});
    std::uniform_int_distribution<std::size_t> dist_prefixes(0, prefixes.size() - 1);
    std::uniform_int_distribution<int> dist_user(1001, 1200);

    const std::vector<float> time_increase_intervals{0, 10, 60};
    const std::vector<float> time_increase_densities{20, 3, 1};
    std::piecewise_linear_distribution<float> dist_time_increase(time_increase_intervals.begin(), time_increase_intervals.end(), time_increase_densities.begin());

    std::array<Generator, 3> postfix_generators{
        Generator{verbs, gen, 50, 150},
        Generator{dialogs, gen, 30, 60},
        Generator{content, gen, 500, 5000}};

    int request = 1'000'000;
    auto time = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());

    for (int i = 0; i < 10; ++i) {
        auto pg{postfix_generators[dist_source(gen)]};
        time += std::chrono::seconds{static_cast<int>(dist_time_increase(gen))};

        std::cout << date::format("%F %T", time) << '\t'
                  << ++request << '\t'
                  << pg.duration() << '\t'
                  << dist_user(gen) << '\t'
                  << prefixes[dist_prefixes(gen)] << "." << pg.postfix() << '\n';
    }
}
