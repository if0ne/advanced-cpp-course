#include "pch.h"
#include <vector>

#include "../QuickSort/Quicksort.h"

class SupportClass {
public:
    SupportClass(const SupportClass& other) : i(other.i) {
        *i += 1;
    }

    SupportClass(SupportClass&& other) {
        i = other.i;
        other.i = nullptr;
    }

    SupportClass(int* i) : i(i) {
        *i += 1;
    }

    ~SupportClass() {
        if (i) *i -= 1;
    }

    SupportClass& operator=(const SupportClass& other) {
        return SupportClass(other);
    }

    SupportClass& operator=(SupportClass&& other) {
        return SupportClass(other);
    }

private:
    int* i;
};

TEST(SwapTest, TestSwapByTestTest) {
    int i = 0;

    {
        SupportClass left{ &i };
        EXPECT_EQ(i, 1);

        SupportClass right{ &i };
        EXPECT_EQ(i, 2);

        SupportClass temp(std::move(left));
        EXPECT_EQ(i, 2);

        left = std::move(right);
        EXPECT_EQ(i, 2);

        right = std::move(temp);
        EXPECT_EQ(i, 2);
    }

    EXPECT_EQ(i, 0);
}

bool compareIntAsc(int a, int b) {
    return a < b;
}

bool compareIntDesc(int a, int b) {
    return a > b;
}

TEST(SelectPivot, FirstElementAsPivotTest) {
    std::vector<int> a = { 2, 3, 1 };

    int* pivot1 = select_pivot(a.data(), a.data() + a.size() - 1, compareIntAsc);
    int* pivot2 = select_pivot(a.data(), a.data() + a.size() - 1, compareIntDesc);

    EXPECT_EQ(pivot1, pivot2);
    EXPECT_EQ(*pivot1, 2);
    EXPECT_EQ(*pivot2, 2);
}

TEST(SelectPivot, LastElementAsPivotTest) {
    std::vector<int> a = { 1, 3, 2 };

    int* pivot1 = select_pivot(a.data(), a.data() + a.size() - 1, compareIntAsc);
    int* pivot2 = select_pivot(a.data(), a.data() + a.size() - 1, compareIntDesc);

    EXPECT_EQ(pivot1, pivot2);
    EXPECT_EQ(*pivot1, 2);
    EXPECT_EQ(*pivot2, 2);
}

TEST(SelectPivot, MiddleElementAsPivotTest) {
    std::vector<int> a = { 1, 2, 3 };

    int* pivot1 = select_pivot(a.data(), a.data() + a.size() - 1, compareIntAsc);
    int* pivot2 = select_pivot(a.data(), a.data() + a.size() - 1, compareIntDesc);

    EXPECT_EQ(pivot1, pivot2);
    EXPECT_EQ(*pivot1, 2);
    EXPECT_EQ(*pivot2, 2);
}

TEST(Partition, PartitionTest) {
    std::vector<int> a = generate(1000);

    int* pivot = partition(a.data(), a.data() + a.size() - 1, compareIntAsc);

    for (int* i = a.data(); i < pivot; i++) {
        EXPECT_LE(*i, *pivot);
    }

    for (int* i = pivot + 1; i < a.data() + a.size() - 1; i++) {
        EXPECT_LE(*pivot, *i);
    }
}

TEST(InsertionSort, InsertionSortAscTest) {
    std::vector<int> a = generate(1000);

    insertion_sort(a.data(), a.data() + a.size() - 1, compareIntAsc);
    for (int* i = a.data(); i < a.data() + a.size() - 2; i++) {
        EXPECT_LE(*i, *(i + 1));
    }
}

TEST(InsertionSort, InsertionSortDescTest) {
    std::vector<int> a = generate(1000);

    insertion_sort(a.data(), a.data() + a.size() - 1, compareIntDesc);
    for (int* i = a.data(); i < a.data() + a.size() - 2; i++) {
        EXPECT_GE(*i, *(i + 1));
    }
}

TEST(InsertionSort, InsertionSortEmptyTest) {
    std::vector<int> a;

    insertion_sort(a.data(), a.data(), compareIntAsc);
    EXPECT_TRUE(a.empty());
}

TEST(QuickSort, QuickSortAscTest) {
    std::vector<int> a = generate(1000);

    quick_sort(a.data(), a.data() + a.size() - 1, compareIntAsc);
    for (int* i = a.data(); i < a.data() + a.size() - 2; i++) {
        EXPECT_LE(*i, *(i + 1));
    }
}

TEST(QuickSort, QuickSortDescTest) {
    std::vector<int> a = generate(1000);

    quick_sort_optimized(a.data(), a.data() + a.size() - 1, compareIntDesc);
    for (int* i = a.data(); i < a.data() + a.size() - 2; i++) {
        EXPECT_GE(*i, *(i + 1));
    }
}

TEST(QuickSort, QuickSortEmptyTest) {
    std::vector<int> a;

    quick_sort_optimized(a.data(), a.data(), compareIntAsc);
    EXPECT_TRUE(a.empty());
}