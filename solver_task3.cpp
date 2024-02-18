#include "helper.h"

using namespace std;

vector<int> greedy_list;
mt19937 gen32(chrono::steady_clock::now().time_since_epoch().count());

void two_la1_algo(int cpu_amount, const string& filename) {

  cout << "2-LA1 for " << filename << endl;

  ifstream fin(filename);
  int sums[cpu_amount + 1];
  vector<int> values;

  for (int i = 0; i <= cpu_amount; i++) {
    sums[i] = 0;
  }

  string str;
  bool initial_read;
  int px, x;
  while (true) {
    getline(fin, str);
    if (str.empty()) break;
    x = strtol(str.c_str(), nullptr, 10);
    values.push_back(x);

    if (initial_read) {
      if (3 * (sums[0] + px) <= 2 * (sums[0] + sums[1] + px + x)) sums[0] += px;
      else sums[1] += px;
    }
    initial_read = true;
    px = x;

  }

  if (sums[0] < sums[1]) sums[0] += x;
  else sums[1] += x;

//  greedy_list.push_back(max(sums[0], sums[1]));
  cout << fixed << setprecision(10) << (double)max(sums[0], sums[1]) / (int)Helper::get_LB_task2(2, 1, 0, values) << '\n';
}

void three_la1_algo(int cpu_amount, const string& filename) {

  cout << "3-LA1 for " << filename << endl;

  ifstream fin(filename);
  int sums[cpu_amount + 1];
  vector<int> values;

  for (int i = 0; i <= cpu_amount; i++) {
    sums[i] = 0;
  }

  string str;
  bool initial_read;
  int px, x;
  while (true) {
    getline(fin, str);
    if (str.empty()) break;
    x = strtol(str.c_str(), nullptr, 10);
    values.push_back(x);

    if (initial_read) {
      if (33 * (sums[0] + px) <= 16 * (sums[0] + sums[1] + sums[2] + px + x)) sums[0] += px;
      else if (33 * (sums[1] + px) <= 15 * (sums[0] + sums[1] + sums[2] + px + x)) sums[1] += px;
      else sums[2] += px;
    }
    initial_read = true;
    px = x;

  }

  if (sums[0] <= sums[1] && sums[0] <= sums[2]) sums[0] += x;
  else if (sums[1] <= sums[0] && sums[1] <= sums[2]) sums[1] += x;
  else sums[2] += x;

//  greedy_list.push_back(max({sums[0], sums[1], sums[2]}));
  cout << fixed << setprecision(10) << (double)max({sums[0], sums[1], sums[2]}) / (int)Helper::get_LB_task2(3, 1, 0, values) << '\n';
}

void print_ans(const vector<int>& vc, const string& name) {
  cout << name << " = [";
  for (int i = 0; i < vc.size(); i++) {
    cout << vc[i];
    if (i < vc.size() - 1) cout << ", ";
  }
  cout << "]" << endl;
}

int main() {

  vector<int> pr = {3, 6, 10, 15};

  for (auto cr : pr) {
    int number_amount;
    string proc_amount = to_string(cr), file;

    greedy_list.clear();

    for (number_amount = 1; number_amount <= 10; number_amount++) {
      file = "tests/" + proc_amount + "/random/output" + to_string(number_amount) + ".txt";
      two_la1_algo(cr, file);
      three_la1_algo(cr, file);
    }
//    print_ans(greedy_list, "2-LA1");
    cout << endl;
  }

  return 0;
}
