#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <sys/time.h>
#include <chrono>

using namespace std;

enum Strategy {
  RANDOM_SHUFFLE,
  PRIORITY_MAX,
  PRIORITY_MIN,
  JUMPS,
};

int elements_amount;
int working_array[2001];
int sums[51], maxs[51], sizes[51];
int distribution[51][2001];
int saved_best_result, saved_best_sizes[51];
int saved_best_distribution[51][2001];
struct timeval time_begin, time_end;
bool time_return;
int sec_to_return = 10;
mt19937 gen32(chrono::steady_clock::now().time_since_epoch().count());
vector<int> lower_bounds_list, sols_list;

int get_LB(int m, vector<int> overall) {
  int n = overall.size();
  if (n <= m) return 0;
  sort(overall.begin(), overall.end());
  reverse(overall.begin(), overall.end());
  int o1 = overall[m];
  int o2 = 0;
  for (int i = m; i < n; i++) {
    o2 += overall[i];
  }
  if (o2 % m == 0) o2 /= m;
  else o2 = o2 / m + 1;
  if (n <= 2 * m) return max(o1, o2);
  int o3 = overall[2 * m - 1] + overall[2 * m];
  return max({o1, o2, o3});
}

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

void generate_test(int amount, int x_coef, int x_plus, Strategy strategy,
                   const string& test_file) {
  ofstream ftest(test_file);
  vector<int> overall;

  for (int i = 1; i <= amount; i++) {
    int x = gen32() % x_coef + x_plus;
    overall.push_back(x);
  }

  if (strategy == RANDOM_SHUFFLE) {
    shuffle(overall.begin(),
            overall.end(),
            std::mt19937(std::random_device()()));
  } else if (strategy == PRIORITY_MAX) {
    int swaps = 2 * amount;
    sort(overall.begin(), overall.end());
    reverse(overall.begin(), overall.end());
    while (swaps > 0) {
      for (int i = 0; i < amount - 1; i++) {
        if (gen32() % 2 == 0) {
          swap(overall[i], overall[i + 1]), swaps--;
          if (swaps == 0) break;
        }
      }
    }
  } else if (strategy == PRIORITY_MIN) {
    int swaps = 2 * amount;
    sort(overall.begin(), overall.end());
    while (swaps > 0) {
      for (int i = 0; i < amount - 1; i++) {
        if (gen32() % 2 == 0) {
          swap(overall[i], overall[i + 1]), swaps--;
          if (swaps == 0) break;
        }
      }
    }
  } else if (strategy == JUMPS) {
    int border = overall.size() / 2;
    sort(overall.begin(), overall.end());
    vector<int> v1, v2;
    for (int i = 0; i < border; i++) v1.push_back(overall[i]);
    for (int i = border; i < overall.size(); i++) v2.push_back(overall[i]);
    shuffle(v1.begin(), v1.end(), std::mt19937(std::random_device()()));
    shuffle(v2.begin(), v2.end(), std::mt19937(std::random_device()()));
    for (int i = 0; i < overall.size(); i++) {
      if (i % 2 == 0) {
        if (!v1.empty()) overall[i] = v1.back(), v1.pop_back();
        else overall[i] = v2.back(), v2.pop_back();
      }
      else {
        if (!v2.empty()) overall[i] = v2.back(), v2.pop_back();
        else overall[i] = v1.back(), v1.pop_back();
      }
    }
  }

  for (auto f : overall) {
    ftest << f << '\n';
  }
}

void solve_test(int cpu_amount, int amount, int solve_sec,
                const string& info_file, const string& test_file) {

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

  int sum = 0, mx = 0;
  elements_amount = overall.size();
  for (int i = 0; i < elements_amount; i++) {
    working_array[i] = overall[i];
    sum += overall[i];
    mx = max(mx, overall[i]);
    saved_best_sizes[0]++;
    saved_best_distribution[0][i] = overall[i];
  }

  int LB = get_LB(cpu_amount, overall);
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

int main() {
  srand(time(nullptr));

  vector<int> pr = {3, 6, 10, 15};

//  for (auto cr : pr) {
//    int number_amount = 1;
//    string proc_amount = to_string(cr);
//    for (int elements = 100; elements <= 1000; elements += 100) {
//      generate_test(elements, elements * elements, 1, RANDOM_SHUFFLE,
//                    "tests/" + proc_amount + "/random/output" + to_string(number_amount) + ".txt");
//      generate_test(elements, elements * elements, 1, PRIORITY_MIN,
//                    "tests/" + proc_amount + "/min/output" + to_string(number_amount) + ".txt");
//      generate_test(elements, elements * elements, 1, PRIORITY_MAX,
//                    "tests/" + proc_amount + "/max/output" + to_string(number_amount) + ".txt");
//      generate_test(elements, elements * elements, 1, JUMPS,
//                    "tests/" + proc_amount + "/jumps/output" + to_string(number_amount) + ".txt");
//      number_amount++;
//    }
//  }

  int solve_sec;

  for (auto cr : pr) {
    int number_amount = 1;
    string proc_amount = to_string(cr);
    lower_bounds_list.clear(), sols_list.clear();
    for (int elements = 100; elements <= 1000; elements += 100) {
      if (elements <= 500) solve_sec = 30;
      else solve_sec = 60;
      solve_test(cr, elements, solve_sec,
                 "tests/" + proc_amount + "/random/test" + to_string(number_amount) + ".txt",
                 "tests/" + proc_amount + "/random/output" + to_string(number_amount) + ".txt");
      number_amount++;
    }
    cout << "LB: [";
    for (int i = 0; i < lower_bounds_list.size(); i++) {
      cout << lower_bounds_list[i];
      if (i < lower_bounds_list.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    cout << "SOLS: [";
    for (int i = 0; i < sols_list.size(); i++) {
      cout << sols_list[i];
      if (i < sols_list.size() - 1) cout << ", ";
    }
    cout << "]" << endl << endl;
  }

  for (auto cr : pr) {
    int number_amount = 1;
    string proc_amount = to_string(cr);
    lower_bounds_list.clear(), sols_list.clear();
    for (int elements = 100; elements <= 1000; elements += 100) {
      if (elements <= 500) solve_sec = 30;
      else solve_sec = 60;
      solve_test(cr, elements, solve_sec,
                 "tests/" + proc_amount + "/min/test" + to_string(number_amount) + ".txt",
                 "tests/" + proc_amount + "/min/output" + to_string(number_amount) + ".txt");
      number_amount++;
    }
    cout << "LB: [";
    for (int i = 0; i < lower_bounds_list.size(); i++) {
      cout << lower_bounds_list[i];
      if (i < lower_bounds_list.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    cout << "SOLS: [";
    for (int i = 0; i < sols_list.size(); i++) {
      cout << sols_list[i];
      if (i < sols_list.size() - 1) cout << ", ";
    }
    cout << "]" << endl << endl;
  }

  for (auto cr : pr) {
    int number_amount = 1;
    string proc_amount = to_string(cr);
    lower_bounds_list.clear(), sols_list.clear();
    for (int elements = 100; elements <= 1000; elements += 100) {
      if (elements <= 500) solve_sec = 30;
      else solve_sec = 60;
      solve_test(cr, elements, solve_sec,
                 "tests/" + proc_amount + "/max/test" + to_string(number_amount) + ".txt",
                 "tests/" + proc_amount + "/max/output" + to_string(number_amount) + ".txt");
      number_amount++;
    }
    cout << "LB: [";
    for (int i = 0; i < lower_bounds_list.size(); i++) {
      cout << lower_bounds_list[i];
      if (i < lower_bounds_list.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    cout << "SOLS: [";
    for (int i = 0; i < sols_list.size(); i++) {
      cout << sols_list[i];
      if (i < sols_list.size() - 1) cout << ", ";
    }
    cout << "]" << endl << endl;
  }

  for (auto cr : pr) {
    int number_amount = 1;
    string proc_amount = to_string(cr);
    lower_bounds_list.clear(), sols_list.clear();
    for (int elements = 100; elements <= 1000; elements += 100) {
      if (elements <= 500) solve_sec = 30;
      else solve_sec = 60;
      solve_test(cr, elements, solve_sec,
                 "tests/" + proc_amount + "/jumps/test" + to_string(number_amount) + ".txt",
                 "tests/" + proc_amount + "/jumps/output" + to_string(number_amount) + ".txt");
      number_amount++;
    }
    cout << "LB: [";
    for (int i = 0; i < lower_bounds_list.size(); i++) {
      cout << lower_bounds_list[i];
      if (i < lower_bounds_list.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    cout << "SOLS: [";
    for (int i = 0; i < sols_list.size(); i++) {
      cout << sols_list[i];
      if (i < sols_list.size() - 1) cout << ", ";
    }
    cout << "]" << endl << endl;
  }

  return 0;
}
