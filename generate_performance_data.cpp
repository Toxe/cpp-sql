#include <array>
#include <iostream>
#include <random>
#include <string>

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
    std::uniform_int_distribution<> dist_time_increase(0, 60);

    std::array<Generator, 3> postfix_generators{
        Generator{verbs, gen, 50, 150},
        Generator{dialogs, gen, 30, 60},
        Generator{content, gen, 500, 5000}};

    int request = 1'000'000;
    int time = 1'000'000'000;

    for (int i = 0; i < 10; ++i) {
        auto pg{postfix_generators[dist_source(gen)]};

        ++request;
        time += dist_time_increase(gen);

        std::cout << request << ',' << time << ',' << prefixes[dist_prefixes(gen)] << "." << pg.postfix() << ',' << pg.duration() << '\n';
    }
}
