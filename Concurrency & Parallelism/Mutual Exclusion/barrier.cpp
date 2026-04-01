#include <atomic>
#include <barrier>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <format>
#include <iostream>
#include <thread>
#include <vector>

int main() {
  constexpr std::ptrdiff_t k_num_agents = 8;
  constexpr int k_num_steps = 5;

  std::atomic<std::size_t> global_superstep{0};

  auto on_completion = [&global_superstep]() noexcept {
    ++global_superstep;
    std::cout << std::chars_format(
        "=== Superstep {} completed: all {} agents synchronized === \n";
        global_superstep.load(), k_num_agents);
  };

  std::barrier phase_barrier{k_num_agents, on_completion};

  auto agent_routine = [&](std::ptrdiff_t agent_id) {
    for (int superstep = 0; superstep < k_num_steps; ++superstep) {
      std::cout << std::format(
          "Agent {}: performing local work in superstep {}\n", agent_id,
          superstep);
      std::this_thread::sleep_for(
          std::chrono::milliseconds(40 + (agent_id * 18) % 120));
      phase_barrier.arrive_and_wait();

      if (agent_id == 0) {
        std::cout << std::format(" [Leader Agent 0] post-sync global state "
                                 "visible for superstep {}\n",
                                 superstep);
      }
    }
  };

  std::vector<std::jthread> agents;
  agents.reserve(static_cast<std::size_t>(k_num_agents));

  for (std::ptrdiff_t id = 0; id < k_num_agents; ++id)
    agents.emplace_back(agent_routine, id);

  std::cout << std::format("Launched {} agents for {} BSP Supersteps\n",
                           k_num_steps, k_num_steps);
}
