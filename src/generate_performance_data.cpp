#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include <date.h>
#include <tz.h>

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

int main(int argc, char* argv[])
{
    if (argc < 2)
        throw std::runtime_error{"missing number of datasets"};

    const int num_datasets = std::stoi(argv[1]);

    const std::vector<std::string> prefixes{"doc", "project", "user", "collection", "file"};
    const std::vector<std::string> verbs{"create", "delete", "update", "move", "download", "archive"};
    const std::vector<std::string> dialogs{"dialog_create", "dialog_delete", "dialog_show", "dialog_move", "dialog_download"};
    const std::vector<std::string> content{"view", "view_all", "view_archive"};
    const std::vector<std::string> user_agents{
        "Mozilla/5.0 (iPhone; CPU iPhone OS 12_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148",
        "Mozilla/5.0 (Linux; U; Android 2.2) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10; rv:33.0) Gecko/20100101 Firefox/33.0",
        "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_6; en-en) AppleWebKit/533.19.4 (KHTML, like Gecko) Version/5.0.3 Safari/533.19.4",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36",
        "Mozilla/5.0 CK={} (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"};

    std::random_device rd;
    std::mt19937 gen(rd());

    std::discrete_distribution<std::size_t> dist_source({60, 10, 30});
    std::uniform_int_distribution<std::size_t> dist_prefixes(0, prefixes.size() - 1);
    std::uniform_int_distribution<std::size_t> dist_user_agents(0, user_agents.size() - 1);
    std::uniform_int_distribution<int> dist_user(1001, 1200);
    std::uniform_int_distribution<int> dist_ip_address(0, 255);

    const std::vector<float> time_increase_intervals{0, 10, 60};
    const std::vector<float> time_increase_densities{20, 3, 1};
    std::piecewise_linear_distribution<float> dist_time_increase(time_increase_intervals.begin(), time_increase_intervals.end(), time_increase_densities.begin());

    std::array<Generator, 3> postfix_generators{
        Generator{verbs, gen, 50, 150},
        Generator{dialogs, gen, 30, 60},
        Generator{content, gen, 500, 5000}};

    int request = 1'000'000;
    auto time = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());

    for (int i = 0; i < num_datasets; ++i) {
        auto pg{postfix_generators[dist_source(gen)]};
        time += std::chrono::seconds{static_cast<int>(dist_time_increase(gen))};

        std::cout << date::format("%F %T", time) << '\t'
                  << ++request << '\t'
                  << pg.duration() << '\t'
                  << dist_user(gen) << '\t'
                  << "192.168." << dist_ip_address(gen) << '.' << dist_ip_address(gen) << '\t'
                  << prefixes[dist_prefixes(gen)] << "." << pg.postfix() << '\t'
                  << user_agents[dist_user_agents(gen)] << '\n';
    }
}
