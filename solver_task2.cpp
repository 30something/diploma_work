#include "helper.h"

using namespace std;

vector<double> main_algo_list;
vector<double> greedy_list[4];
vector<double> dummy_list;
mt19937 gen32(chrono::steady_clock::now().time_since_epoch().count());

void dummy_distribution(const Params& params, const string& filename) {

  // choose 1, 2, 3, ..., 1, 2, 3, ...
  cout << "dummy for " << filename << endl;

  int cpu_amount = params.cpu_amount;
  int m1 = params.m1;
  int k = params.k;
  int x = params.x;
  int R = params.R;
  int S = params.S;
  int m2 = R * (k * (S - 1) + x);

  ifstream fin(filename);
  double sums[cpu_amount + 1];
  double speeds[cpu_amount + 1];

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = 0;
    if (i <= m1 + m2) speeds[i] = 1;
    else speeds[i] = S;
  }

  string str;
  int cur_index = 1;
  while (true) {
    getline(fin, str);
    if (str.empty()) break;
    double val = strtol(str.c_str(), nullptr, 10);

    sums[cur_index] += val / speeds[cur_index];
    cur_index++;
    if (cur_index > cpu_amount) cur_index = 1;

  }

  double gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    gen_mx = max(gen_mx, sums[i]);
  }

  dummy_list.push_back(gen_mx);

}

void greedy_algo1(const Params& params, const string& filename) {

  // choose minimal load
  cout << "greedy1 for " << filename << endl;

  int cpu_amount = params.cpu_amount;
  int m1 = params.m1;
  int k = params.k;
  int x = params.x;
  int R = params.R;
  int S = params.S;
  int m2 = R * (k * (S - 1) + x);

  ifstream fin(filename);
  double sums[cpu_amount + 1];
  double speeds[cpu_amount + 1];

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = 0;
    if (i <= m1 + m2) speeds[i] = 1;
    else speeds[i] = S;
  }

  string str;
  while (true) {
    getline(fin, str);
    if (str.empty()) break;
    double val = strtol(str.c_str(), nullptr, 10);

    int min_index = 1;
    for (int i = 2; i <= cpu_amount; i++) {
      if (sums[i] < sums[min_index]) {
        min_index = i;
      }
    }

    sums[min_index] += val / speeds[min_index];

  }

  double gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    gen_mx = max(gen_mx, sums[i]);
  }

  greedy_list[1].push_back(gen_mx);

}

void greedy_algo2(const Params& params, const string& filename) {

  // choose with minimal adding to system
  cout << "greedy2 for " << filename << endl;

  int cpu_amount = params.cpu_amount;
  int m1 = params.m1;
  int k = params.k;
  int x = params.x;
  int R = params.R;
  int S = params.S;
  int m2 = R * (k * (S - 1) + x);

  ifstream fin(filename);
  double sums[cpu_amount + 1];
  double speeds[cpu_amount + 1];

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = 0;
    if (i <= m1 + m2) speeds[i] = 1;
    else speeds[i] = S;
  }

  string str;
  double cload = 0;
  while (true) {
    getline(fin, str);
    if (str.empty()) break;
    double val = strtol(str.c_str(), nullptr, 10);

    double diff = -1, nload;
    int diff_ind;
    for (int i = 1; i <= cpu_amount; i++) {
      nload = sums[i] + val / speeds[i];
      for (int j = 1; j <= cpu_amount; j++) {
        if (i == j) continue;
        nload = max(nload, sums[j]);
      }
      if (diff == -1 || nload - cload < diff) {
        diff = nload - cload;
        diff_ind = i;
      }
    }

    sums[diff_ind] += val / speeds[diff_ind];
    cload += diff;

  }

  double gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    gen_mx = max(gen_mx, sums[i]);
  }

  greedy_list[2].push_back(gen_mx);

}

void greedy_algo3(const Params& params, const string& filename) {

  // combination of #2 + #1
  cout << "greedy3 for " << filename << endl;

  int cpu_amount = params.cpu_amount;
  int m1 = params.m1;
  int k = params.k;
  int x = params.x;
  int R = params.R;
  int S = params.S;
  int m2 = R * (k * (S - 1) + x);

  ifstream fin(filename);
  double sums[cpu_amount + 1];
  double speeds[cpu_amount + 1];

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = 0;
    if (i <= m1 + m2) speeds[i] = 1;
    else speeds[i] = S;
  }

  string str;
  double cload = 0;
  while (true) {
    getline(fin, str);
    if (str.empty()) break;
    double val = strtol(str.c_str(), nullptr, 10);

    int diff_ind;
    double diff = -1, nload;
    for (int i = 1; i <= cpu_amount; i++) {
      nload = sums[i] + val / speeds[i];
      for (int j = 1; j <= cpu_amount; j++) {
        if (i == j) continue;
        nload = max(nload, sums[j]);
      }
      if (diff == -1 || nload - cload < diff || (abs(nload - cload - diff) < 1e-6 && sums[i] < sums[diff_ind])) {
        diff = nload - cload;
        diff_ind = i;
      }
    }

    sums[diff_ind] += val / speeds[diff_ind];
    cload += diff;

  }

  double gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    gen_mx = max(gen_mx, sums[i]);
  }

  greedy_list[3].push_back(gen_mx);

}

void main_algo(const Params& params, const string& filename) {

  // main approximate algorithm
  cout << "main approximate for " << filename << endl;

  int cpu_amount = params.cpu_amount;
  int m1 = params.m1;
  int k = params.k;
  int x = params.x;
  int R = params.R;
  int l = params.l;
  int S = params.S;
  double phi = params.phi, psi = params.psi, B = params.B;
  int m2 = R * (k * (S - 1) + x);

  ifstream fin(filename);
  double sums[cpu_amount + 1];
  double speeds[cpu_amount + 1];
  int c1 = 1, c2 = k * (S - 1) + 1, c3 = 0;

  vector<int> standard(m1);
  vector<vector<int>> reserved(R, vector<int>(k * (S - 1) + x));
  // fast CPUs are always from (m1 + m2 + 1) to cpu_amount

  int group = 0, id = 0;
  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = 0;
    if (i <= m1 + m2) speeds[i] = 1;
    else speeds[i] = S;
    if (i <= m1) standard[i - 1] = i;
    else if (i <= m1 + m2) {
      reserved[group][id] = i;
      if (id == k * (S - 1) + x - 1) group++, id = 0;
      else id++;
    }
  }

  string str;
  vector<int> overall_values;
  while (true) {
    getline(fin, str);
    if (str.empty()) break;
    int val = strtol(str.c_str(), nullptr, 10);
    double dval = val;
    overall_values.push_back(val);

    /// Step 1
    bool task_placed = false;
    double LB = Helper::get_LB_task2(cpu_amount, S, k, overall_values);

    /// Step 2
    // try to place to fast CPU
    for (int i = m1 + m2 + 1; i <= cpu_amount; i++) {
      if (sums[i] + dval / speeds[i] <= B * LB) {
        sums[i] += dval / speeds[i];
        task_placed = true;
        break;
      }
    }

    if (task_placed) continue;

    // try to place to standard CPU
    for (auto f : standard) {
      if (sums[f] + dval / speeds[f] <= B * LB) {
        sums[f] += dval / speeds[f];
        task_placed = true;
        break;
      }
    }

    if (task_placed) continue;

    // only first two steps must be performed if R == 0
    assert(R > 0);

    /// Step 3
    bool is_big = (dval > psi * B * LB);
    bool is_medium = (dval > phi * B * LB && !is_big);
    bool is_small = (!is_big && !is_medium);

    place_again:

    if (is_big && c1 <= k * (S - 1)) {
      /// 3a

      int first_reserved = reserved[0][c1 - 1];
      sums[first_reserved] += dval / speeds[first_reserved];
      task_placed = true;

      double min_load;
      int min_load_ind = -1;
      for (int i = 0; i < standard.size(); i++) {
        if (min_load_ind == -1 || sums[standard[i]] < min_load) {
          min_load = sums[standard[i]];
          min_load_ind = i;
        }
      }

      swap(standard[min_load_ind], reserved[0][c1 - 1]);
      c1++;

    } else if (is_medium && c2 <= k * (S - 1) + x) {
      /// 3b

      int second_reserved = reserved[0][c2 - 1];
      sums[second_reserved] += dval / speeds[second_reserved];
      task_placed = true;

      c3++;

      if (c3 > l) {
        double min_load;
        int min_load_ind = -1;
        for (int i = 0; i < standard.size(); i++) {
          if (min_load_ind == -1 || sums[standard[i]] < min_load) {
            min_load = sums[standard[i]];
            min_load_ind = i;
          }
        }

        swap(standard[min_load_ind], reserved[0][c2 - 1]);
        c2++;
        c3 = 0;
      }

    } else {
      /// 3c

      vector<int> saved_front_cpus = reserved[0];
      reserved.erase(reserved.begin());
      reserved.push_back(saved_front_cpus);

      c1 = 1;
      c2 = k * (S - 1) + 1;
      c3 = 0;

      goto place_again;

    }

    assert(task_placed);

  }

  double gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    gen_mx = max(gen_mx, sums[i]);
  }

  main_algo_list.push_back(gen_mx);

}

void print_ans(const vector<double>& vc, const string& name) {
  cout.precision(6);
  cout << name << " = [";
  for (int i = 0; i < vc.size(); i++) {
    cout << fixed << vc[i];
    if (i < vc.size() - 1) cout << ", ";
  }
  cout << "]" << endl;
}

void test_by_strategy(const string& strategy) {

  int S = 2;
  vector<int> ms = {10, 30, 60, 90};
  vector<int> m1s = {6, 6, 16, 18};
  vector<int> ks = {4, 4, 5, 8};
  vector<int> Rs = {0, 2, 3, 4};
  vector<int> ls = {1, 2, 2, 3};
  vector<int> xs = {4, 6, 8, 8};
  vector<double> phis = {1., 1. / 6, 1. / 4, 1. / 6};
  vector<double> psis = {1., 1. / 3, 1. / 3, 1. / 4};
  vector<double> B = {2.75, 4.75, 3.94, 7.35};

  for (int i = 0; i < ms.size(); i++) {
    string proc_amount = to_string(ms[i]), file;

    main_algo_list.clear();
    for (int j = 1; j <= 3; j++) greedy_list[j].clear();
    dummy_list.clear();

    for (int number_amount = 1; number_amount <= 10; number_amount++) {
      file = "tests/" + proc_amount + "/" + strategy + "/output" + to_string(number_amount) + ".txt";
      Params cur_params = {ms[i], S, m1s[i], ks[i], Rs[i], ls[i], xs[i], phis[i], psis[i], B[i]};

      main_algo(cur_params, file);
      greedy_algo1(cur_params, file);
      greedy_algo2(cur_params, file);
      greedy_algo3(cur_params, file);
      dummy_distribution(cur_params, file);
    }
    this_thread::sleep_for(std::chrono::milliseconds(2000));

    print_ans(main_algo_list, "Main");
    print_ans(greedy_list[1], "G1");
    print_ans(greedy_list[2], "G2");
    print_ans(greedy_list[3], "G3");
    print_ans(dummy_list, "Dummy");

    cout << endl;
  }

}

int main() {

  test_by_strategy("random");
  test_by_strategy("min");
  test_by_strategy("max");
  test_by_strategy("jumps");

  return 0;
}
