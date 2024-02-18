#include "helper.h"

using namespace std;

int elements_amount;
int working_array[kMaxTasksAmount];
double sums[kMaxCpuAmount];
int sizes[kMaxCpuAmount], speeds[kMaxCpuAmount];
int distribution[kMaxCpuAmount][kMaxTasksAmount];
double saved_best_result;
int saved_best_sizes[kMaxCpuAmount];
double start_distribution_sums[kMaxCpuAmount];
int saved_best_distribution[kMaxCpuAmount][kMaxTasksAmount];

struct timeval time_begin, time_end;
bool time_return;
int sec_to_return = 10;

void solve(int cpu_amount, int current_pos, double current_oc) {

  if (time_return || current_oc >= saved_best_result) return;

  gettimeofday(&time_end, nullptr);
  if (time_end.tv_sec - time_begin.tv_sec > sec_to_return) {
    time_return = true;
    return;
  }

  if (current_pos == elements_amount) {
    double current_result = 0;
    for (int i = 0; i < cpu_amount; i++) {
      current_result = max(current_result, sums[i]);
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
  int cpu_number;
  double new_sum, old_sum;

  vector<pair<double, int>> prior_cpus(cpu_amount);
  for (int i = 0; i < cpu_amount; i++) {
    prior_cpus[i] = {sums[i] + (double)x / speeds[i], i};
  }

  sort(prior_cpus.begin(), prior_cpus.end());

  for (int i = 0; i < cpu_amount; i++) {
    cpu_number = prior_cpus[i].second;
    old_sum = sums[cpu_number];
    new_sum = prior_cpus[i].first;

    if (new_sum >= saved_best_result) return;

    sums[cpu_number] = new_sum;
    distribution[cpu_number][sizes[cpu_number]] = x;
    sizes[cpu_number]++;
    solve(cpu_amount, current_pos + 1, max(current_oc, new_sum));

    sums[cpu_number] = old_sum;
    sizes[cpu_number]--;
  }
}

void solve_test(const Params& params, int amount, int solve_sec,
                ifstream& ftest, ofstream& finfo,
                vector<double>& lower_bounds_list, vector<double>& sols_list) {

  int cpu_amount = params.cpu_amount;
  int m1 = params.m1;
  int k = params.k;
  int x = params.x;
  int R = params.R;
  int S = params.S;
  int m2 = R * (k * (S - 1) + x);

  vector<int> overall;
  int read_amount, inp;

  ftest >> read_amount;
  assert(read_amount == amount);
  for (int i = 1; i <= amount; i++) {
    ftest >> inp;
    overall.push_back(inp);
  }

  for (int i = 0; i < cpu_amount; i++) {
    saved_best_sizes[i] = 0;
    sums[i] = 0;
    sizes[i] = 0;
    start_distribution_sums[i] = 0;
    if (i < m1 + m2) speeds[i] = 1;
    else speeds[i] = S;  // S > 1
  }

  // distribution based on greedy v1

  int distribution_index;
  double sum = 0;
  elements_amount = overall.size();
  for (int i = 0; i < elements_amount; i++) {
    working_array[i] = overall[i];

    int min_ind = -1;
    double min_load;
    for (int j = 0; j < cpu_amount; j++) {
      if (min_ind == -1 || start_distribution_sums[j] < min_load) {
        min_ind = j;
        min_load = start_distribution_sums[j];
      }
    }

    start_distribution_sums[min_ind] += (double)overall[i] / speeds[min_ind];
    distribution_index = saved_best_sizes[min_ind];
    saved_best_distribution[min_ind][distribution_index] = overall[i];
    saved_best_sizes[min_ind]++;
  }

  for (int i = 0; i < cpu_amount; i++) {
    sum = max(sum, start_distribution_sums[i]);
  }

  double LB = Helper::get_LB_task2(cpu_amount, S, k, overall);
  finfo << "Elements for " << cpu_amount << " CPUs (size = " << overall.size() << "):\n";
  finfo << "Lower bound: " << LB << '\n';

  saved_best_result = sum;
  time_return = false;
  sec_to_return = solve_sec;

  gettimeofday(&time_begin, nullptr);
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
  finfo << endl << endl;

  cout.precision(6);
  cout << "Test with " << amount << " tasks solved" << endl;
  cout << "Lower bound: " << fixed << LB << endl;
  cout << "Solution: " << fixed << saved_best_result << endl << endl;

  lower_bounds_list.push_back(LB);
  sols_list.push_back(saved_best_result);
}

void print_lower_bound_and_solves(const vector<double>& lower_bounds_list,
                                  const vector<double>& sols_list,
                                  ofstream& finfo) {
  finfo.precision(6);
  finfo << "LB = [";
  for (int i = 0; i < lower_bounds_list.size(); i++) {
    finfo << fixed << lower_bounds_list[i];
    if (i < lower_bounds_list.size() - 1) finfo << ", ";
  }
  finfo << "]" << endl;
  finfo << "SOL = [";
  for (int i = 0; i < sols_list.size(); i++) {
    finfo << fixed << sols_list[i];
    if (i < sols_list.size() - 1) finfo << ", ";
  }
  finfo << "]" << endl << endl;
}

void solve_by_strategy(const string& limit, const string& strategy) {
  int solve_sec, S = 2, limit_int;
  vector<int> ms = {10, 30, 60, 90};
  vector<int> m1s = {6, 6, 16, 18};
  vector<int> ks = {4, 4, 5, 8};
  vector<int> Rs = {0, 2, 3, 4};
  vector<int> ls = {1, 2, 2, 3};
  vector<int> xs = {4, 6, 8, 8};
  vector<double> phis = {1., 1. / 6, 1. / 4, 1. / 6};
  vector<double> psis = {1., 1. / 3, 1. / 3, 1. / 4};
  vector<double> B = {2.75, 4.75, 3.94, 7.35};

  vector<double> lower_bounds_list, sols_list;
  lower_bounds_list.reserve(100);
  sols_list.reserve(100);

  string foldername = "tests_" + limit + "/" + strategy;
  string test_file = foldername + "/tests_inputs_" + limit + ".txt";
  string info_file = foldername + "/tests_outputs_task2_" + limit + ".txt";
  ofstream finfo(info_file);
//  ofstream finfo(info_file, ios_base::app);

  if (limit == "n") limit_int = 3000;
  else if (limit == "n2") limit_int = 1000;

  for (int i = 0; i < ms.size(); i++) {

    ifstream ftest(test_file);
    cout << "Moving to " << limit << " limit and " << strategy <<
            " strategy with " << ms[i] << " CPUs\n\n";
    string proc_amount = to_string(ms[i]);
    lower_bounds_list.clear();
    sols_list.clear();

    for (int elements = 100; elements <= limit_int; elements += 100) {
      solve_sec = 10;
      Params params = {ms[i], S, m1s[i], ks[i], Rs[i], ls[i], xs[i], phis[i], psis[i], B[i]};
      solve_test(params, elements, solve_sec,
                 ftest, finfo,
                 lower_bounds_list, sols_list);
    }

    finfo << "limit: " << limit << ", strategy: " << strategy <<
             ", CPUs: " << ms[i] << endl;
    print_lower_bound_and_solves(lower_bounds_list, sols_list, finfo);
    ftest.close();
  }
}

int main() {

  vector<string> limits = {"n", "n2"};

  for (const auto& limit : limits) {
    solve_by_strategy(limit, "random");
    solve_by_strategy(limit, "min");
    solve_by_strategy(limit, "max");
    solve_by_strategy(limit, "jumps");
  }

  return 0;
}
