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
    new_max = max(maxs[cpu_number], x);  // always will return maxs[cpu_number]
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
                ifstream& ftest, ofstream& finfo,
                vector<int>& lower_bounds_list, vector<int>& sols_list) {

  vector<int> overall;
  int read_amount, x;

  ftest >> read_amount;
  assert(read_amount == amount);
  elements_amount = amount;
  for (int i = 1; i <= amount; i++) {
    ftest >> x;
    overall.push_back(x);
  }

  int LB = Helper::get_LB_task1(cpu_amount, overall);
  finfo << "Elements for " << cpu_amount << " CPUs (size = " << overall.size() << "):\n";
  finfo << "Lower bound: " << LB << '\n';

  for (int i = 0; i < cpu_amount; i++) {
    saved_best_sizes[i] = 0;
    sums[i] = maxs[i] = sizes[i] = 0;
  }

  // solve test in offline
  sort(overall.begin(), overall.end());
  reverse(overall.begin(), overall.end());

  // based distribution uses LPT and achieves (4/3 - 1/3m) ratio
  vector<int> current_sums(amount, 0);
  vector<int> current_maxs(amount, 0);
  for (int i = 0; i < amount; i++) {
    working_array[i] = overall[i];
    int ind = 0;
    for (int j = 1; j < cpu_amount; j++) {
      if (current_sums[j] < current_sums[ind]) ind = j;
    }
    current_sums[ind] += overall[i];
    current_maxs[ind] = max(current_maxs[ind], overall[i]);
    saved_best_distribution[ind][saved_best_sizes[ind]] = overall[i];
    saved_best_sizes[ind]++;
  }

  saved_best_result = 0;
  for (int i = 0; i < cpu_amount; i++) {
    saved_best_result = max(saved_best_result, current_sums[i] - current_maxs[i]);
    // distribution of first m tasks
    sums[i] = maxs[i] = overall[i];
    distribution[i][0] = overall[i];
    sizes[i]++;
  }

  gettimeofday(&time_begin, nullptr);
  time_return = false;
  sec_to_return = solve_sec;
  solve(cpu_amount, cpu_amount, 0);
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
  finfo << endl << endl;

  cout << "Test with " << amount << " tasks solved" << endl;
  cout << "Lower bound: " << LB << endl;
  cout << "Solution: " << saved_best_result << endl << endl;

  lower_bounds_list.push_back(LB);
  sols_list.push_back(saved_best_result);

}

void print_lower_bound_and_solves(const vector<int>& lower_bounds_list,
                                  const vector<int>& sols_list,
                                  ofstream& finfo) {
  finfo << "LB = [";
  for (int i = 0; i < lower_bounds_list.size(); i++) {
    finfo << lower_bounds_list[i];
    if (i < lower_bounds_list.size() - 1) finfo << ", ";
  }
  finfo << "]" << endl;
  finfo << "SOL = [";
  for (int i = 0; i < sols_list.size(); i++) {
    finfo << sols_list[i];
    if (i < sols_list.size() - 1) finfo << ", ";
  }
  finfo << "]" << endl << endl;
}

int main() {
//  vector<int> pr = {3, 6, 10, 15, 20, 30};
  vector<int> pr = {75};
  vector<int> lower_bounds_list, sols_list;
  vector<string> strategies = {"random", "min", "max", "jumps"};
//  vector<string> limits = {"n", "n2"};
  vector<string> limits = {"n2"};
  int solve_sec, limit_int = 1000;

  lower_bounds_list.reserve(100);
  sols_list.reserve(100);

  for (const auto& limit : limits) {
    for (const auto& strategy: strategies) {

      string foldername = "tests_" + limit + "/" + strategy;
      string test_file = foldername + "/tests_inputs_" + limit + ".txt";
      string info_file = foldername + "/tests_outputs_task1_" + limit + ".txt";
      ofstream finfo(info_file);
//      ofstream finfo(info_file, ios_base::app);

      if (limit == "n") limit_int = 3000;
      else if (limit == "n2") limit_int = 1000;

      for (auto cr: pr) {

        ifstream ftest(test_file);
        cout << "Moving to " << limit << " limit and " << strategy <<
                " strategy with " << cr << " CPUs\n\n";
        string proc_amount = to_string(cr);
        lower_bounds_list.clear();
        sols_list.clear();

        for (int elements = 100; elements <= limit_int; elements += 100) {
          solve_sec = 30;
          solve_test(cr, elements, solve_sec,
                     ftest, finfo,
                     lower_bounds_list, sols_list);
        }

        finfo << "limit: " << limit << ", strategy: " << strategy <<
                 ", CPUs: " << cr << endl;
        print_lower_bound_and_solves(lower_bounds_list, sols_list, finfo);
        ftest.close();
      }
    }
  }

  return 0;
}
