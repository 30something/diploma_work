#include "helper.h"

using namespace std;

int elements_amount;
int working_array[kMaxTasksAmount];
int sums[kMaxCpuAmount], maxs[kMaxCpuAmount], sizes[kMaxCpuAmount];
int distribution[kMaxCpuAmount][kMaxTasksAmount];
int saved_best_result, saved_best_sizes[kMaxCpuAmount];
int saved_best_distribution[kMaxCpuAmount][kMaxTasksAmount];

struct timeval time_begin, time_end;
bool time_return;
int sec_to_return = 10;

void solve(int cpu_amount, int current_pos, int current_oc) {

  if (time_return) return;

  gettimeofday(&time_end, nullptr);
  if (time_end.tv_sec - time_begin.tv_sec > sec_to_return) {
    time_return = true;
    return;
  }

  if (current_oc >= saved_best_result) return;
  if (current_pos == elements_amount) {
    int current_result = 0;
    for (int i = 0; i < cpu_amount; i++) {
      current_result = max(current_result, sums[i] - maxs[i]);
    }
    if (current_result < saved_best_result) {
      saved_best_result = current_result;
      for (int i = 0; i < cpu_amount; i++) {
        saved_best_sizes[i] = sizes[i];
        for (int j = 0; j < sizes[i]; j++) {
          saved_best_distribution[i][j] = distribution[i][j];
        }
      }
    }
    return;
  }

  int x = working_array[current_pos];
  int new_sum, new_max;
  int old_sum, old_max;
  int cpu_number;

  vector<pair<int, int>> prior_cpus(cpu_amount);
  for (int i = 0; i < cpu_amount; i++) {
    prior_cpus[i] = {sums[i] + x - max(x, maxs[i]), i};
  }

  sort(prior_cpus.begin(), prior_cpus.end());

  for (int i = 0; i < cpu_amount; i++) {
    cpu_number = prior_cpus[i].second;
    old_sum = sums[cpu_number];
    old_max = maxs[cpu_number];

    new_sum = sums[cpu_number] + x;
    new_max = max(maxs[cpu_number], x);
    if (new_sum - new_max >= saved_best_result) continue;

    sums[cpu_number] = new_sum;
    maxs[cpu_number] = new_max;
    distribution[cpu_number][sizes[cpu_number]] = x;
    sizes[cpu_number]++;
    solve(cpu_amount, current_pos + 1, max(current_oc, new_sum - new_max));

    sums[cpu_number] = old_sum;
    maxs[cpu_number] = old_max;
    sizes[cpu_number]--;
  }

}

void solve_test(int cpu_amount, int amount, int solve_sec,
                const string& info_file, const string& test_file,
                vector<int>& lower_bounds_list, vector<int>& sols_list) {

  ofstream finfo(info_file);
  ifstream ftest(test_file);
  vector<int> overall;

  int x;
  for (int i = 1; i <= amount; i++) {
    ftest >> x;
    overall.push_back(x);
  }

  for (int i = 0; i < cpu_amount; i++) {
    saved_best_sizes[i] = 0;
    sums[i] = maxs[i] = sizes[i] = 0;
  }

  // TODO: improve basic distribution
  int sum = 0, mx = 0;
  elements_amount = overall.size();
  for (int i = 0; i < elements_amount; i++) {
    working_array[i] = overall[i];
    sum += overall[i];
    mx = max(mx, overall[i]);
    saved_best_sizes[0]++;
    saved_best_distribution[0][i] = overall[i];
  }

  int LB = Helper::get_LB_task1(cpu_amount, overall);
  finfo << "Elements for " << cpu_amount << " CPUs (size = " << overall.size() << "):\n";
  finfo << "Lower bound: " << LB << '\n';

  saved_best_result = sum - mx;
  gettimeofday(&time_begin, nullptr);
  time_return = false;
  sec_to_return = solve_sec;
  solve(cpu_amount, 0, 0);
  finfo << "Solution: " << saved_best_result << '\n';
  for (int i = 0; i < cpu_amount; i++) {
    finfo << "Cpu #" << i << ": ";
    for (int j = 0; j < saved_best_sizes[i]; j++) {
      finfo << saved_best_distribution[i][j] << ' ';
    }
    finfo << '\n';
  }

  for (auto f : overall) {
    finfo << f << ' ';
  }
  finfo << '\n';

  cout << "Test in " << test_file << " solved" << endl;
  cout << "Lower bound: " << LB << endl;
  cout << "Solution: " << saved_best_result << endl << endl;

  lower_bounds_list.push_back(LB);
  sols_list.push_back(saved_best_result);

}

void print_lower_bound_and_solves(const vector<int>& lower_bounds_list,
                                  const vector<int>& sols_list) {
  cout << "LB = [";
  for (int i = 0; i < lower_bounds_list.size(); i++) {
    cout << lower_bounds_list[i];
    if (i < lower_bounds_list.size() - 1) cout << ", ";
  }
  cout << "]" << endl;
  cout << "SOLS = [";
  for (int i = 0; i < sols_list.size(); i++) {
    cout << sols_list[i];
    if (i < sols_list.size() - 1) cout << ", ";
  }
  cout << "]" << endl << endl;
}

int main() {
  vector<int> pr = {3, 6, 10, 15};
  vector<string> strategies = {"random", "min", "max", "jumps"};
  int solve_sec;

  for (const auto& strategy : strategies) {
    for (auto cr : pr) {
      int number_amount = 1;
      string proc_amount = to_string(cr);
      vector<int> lower_bounds_list, sols_list;

      for (int elements = 100; elements <= 1000; elements += 100) {
        if (elements <= 500) solve_sec = 30;
        else solve_sec = 60;
        solve_test(cr, elements, solve_sec,
                   "tests/" + proc_amount + "/" + strategy + "/test" + to_string(number_amount) + ".txt",
                   "tests/" + proc_amount + "/" + strategy + "/output" + to_string(number_amount) + ".txt",
                   lower_bounds_list, sols_list);
        number_amount++;
      }

      print_lower_bound_and_solves(lower_bounds_list, sols_list);
    }
  }

  return 0;
}
