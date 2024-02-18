#include "helper.h"

enum Strategy {
  RANDOM_SHUFFLE,
  PRIORITY_MAX,
  PRIORITY_MIN,
  JUMPS,
};

mt19937 gen32(chrono::steady_clock::now().time_since_epoch().count());
const string abs_path = filesystem::current_path().string();

void create_folder(const string& folder) {
  filesystem::create_directories(abs_path + "\\" + folder);
}

void generate_test(int amount, int x_coef, int x_plus, Strategy strategy,
                   const string& test_folder, const string& test_file) {

  create_folder(test_folder);
  ofstream ftest;
  if (amount == 100) {
    ftest.open(test_folder + "\\" + test_file);
  } else {
    ftest.open(test_folder + "\\" + test_file, ios_base::app);
  }
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

  ftest << amount << '\n';
  for (auto f : overall) {
    ftest << f << ' ';
  }
  ftest << "\n\n";
}

int main() {

  vector<string> limits = {"n", "n2"};
  vector<int> int_limits = {3000, 1000};

  for (int index = 0; index < limits.size(); index++) {
    string test_filename = "tests_inputs_" + limits[index] + ".txt";
    string testfolder = "tests_" + limits[index];
    for (int elements = 100; elements <= int_limits[index]; elements += 100) {
      int x_coef = elements;
      if (limits[index] == "n2") x_coef = elements * elements;
      generate_test(elements, x_coef, 1, RANDOM_SHUFFLE,
                    testfolder + "/random", test_filename);
      generate_test(elements, x_coef, 1, PRIORITY_MIN,
                    testfolder + "/min", test_filename);
      generate_test(elements, x_coef, 1, PRIORITY_MAX,
                    testfolder + "/max", test_filename);
      generate_test(elements, x_coef, 1, JUMPS,
                    testfolder + "/jumps", test_filename);
    }
  }

  return 0;
}
