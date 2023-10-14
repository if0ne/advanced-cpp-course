#include "pch.h"
#include "../CustomArray/Array.h"

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

TEST(CustomArrayTest, InsertBackTest) {
    Array<int> a{};

    a.insert(1);
    a.insert(2);
    a.insert(3);

    EXPECT_EQ(a.size(), 3);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 2);
    EXPECT_EQ(a[2], 3);
}

TEST(CustomArrayTest, InsertBackClassTest) {
    int i = 0;

    {
        Array<SupportClass> a{};

        a.insert(SupportClass{ &i });
        a.insert(SupportClass{ &i });
        a.insert(SupportClass{ &i });

        EXPECT_EQ(a.size(), 3);
        EXPECT_EQ(i, 3);
    }

    EXPECT_EQ(i, 0);
}

TEST(CustomArrayTest, InsertBackCopyClassTest) {
    int i = 0;

    SupportClass foo = SupportClass{ &i };
    {
        Array<SupportClass> a{};

        a.insert(SupportClass{ foo });
        a.insert(SupportClass{ foo });
        a.insert(SupportClass{ foo });

        EXPECT_EQ(a.size(), 3);
        EXPECT_EQ(i, 4);
    }

    EXPECT_EQ(i, 1);
}

TEST(CustomArrayTest, InsertInMiddleTest) {
    Array<int> a{};

    a.insert(1);
    a.insert(3);
    a.insert(1, 2);

    EXPECT_EQ(a.size(), 3);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 2);
    EXPECT_EQ(a[2], 3);
}

TEST(CustomArrayTest, InsertInMiddleClassTest) {
    int i = 0;

    {
        Array<SupportClass> a{};

        a.insert(SupportClass{ &i });
        a.insert(SupportClass{ &i });
        a.insert(1, SupportClass{ &i });

        EXPECT_EQ(a.size(), 3);
        EXPECT_EQ(i, 3);
    }

    EXPECT_EQ(i, 0);
}

TEST(CustomArrayTest, InsertInMiddleCopyClassTest) {
    int i = 0;

    SupportClass foo = SupportClass{ &i };
    {
        Array<SupportClass> a{};

        a.insert(SupportClass{ &i });
        a.insert(SupportClass{ &i });
        a.insert(1, SupportClass{ foo });

        EXPECT_EQ(a.size(), 3);
        EXPECT_EQ(i, 4);
    }

    EXPECT_EQ(i, 1);
}

TEST(CustomArrayTest, RemoveFromEndTest) {
    Array<int> a{};

    a.insert(1);
    a.insert(2);
    a.insert(3);

    a.remove(2);

    EXPECT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 2);
}

TEST(CustomArrayTest, RemoveFromMiddleTest) {
    Array<int> a{};

    a.insert(1);
    a.insert(2);
    a.insert(3);

    a.remove(1);

    EXPECT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 3);
}

TEST(CustomArrayTest, RemoveFromEndClassTest) {
    int i = 0;

    {
        Array<SupportClass> a{};

        a.insert(SupportClass { &i });
        a.insert(SupportClass { &i });
        a.insert(SupportClass { &i });

        a.remove(2);
        EXPECT_EQ(a.size(), 2);
        EXPECT_EQ(i, 2);
    }
    
    EXPECT_EQ(i, 0);
}

TEST(CustomArrayTest, RemoveFromMiddleClassTest) {
    int i = 0;

    {
        Array<SupportClass> a{};

        a.insert(SupportClass{ &i });
        a.insert(SupportClass{ &i });
        a.insert(SupportClass{ &i });

        a.remove(1);
        EXPECT_EQ(a.size(), 2);
        EXPECT_EQ(i, 2);
    }

    EXPECT_EQ(i, 0);
}

TEST(CustomArrayTest, CheckCapacityGrow) {
    Array<int> a{1};

    a.insert(1);

    EXPECT_EQ(a.size(), 1);
    EXPECT_EQ(a.capacity(), 1);

    a.insert(2);

    EXPECT_EQ(a.size(), 2);
    EXPECT_EQ(a.capacity(), 2);

    a.insert(3);

    EXPECT_EQ(a.size(), 3);
    EXPECT_EQ(a.capacity(), 4);
 
    a.insert(4);

    EXPECT_EQ(a.size(), 4);
    EXPECT_EQ(a.capacity(), 4);
}

TEST(CustomArrayTest, IteratorTest) {
    Array<int> a{};

    a.insert(1);
    a.insert(2);
    a.insert(3);

    auto iter = a.iterator();

    EXPECT_EQ(*iter, 1);
    EXPECT_EQ(iter.hasNext(), true);

    iter.next();

    EXPECT_EQ(*iter, 2);
    EXPECT_EQ(iter.hasNext(), true);

    iter.next();

    EXPECT_EQ(*iter, 3);
    EXPECT_EQ(iter.hasNext(), false);
}

TEST(CustomArrayTest, ReversedIteratorTest) {
    Array<int> a{};

    a.insert(1);
    a.insert(2);
    a.insert(3);

    auto iter = a.reverseIterator();

    EXPECT_EQ(*iter, 3);
    EXPECT_EQ(iter.hasNext(), true);

    iter.next();

    EXPECT_EQ(*iter, 2);
    EXPECT_EQ(iter.hasNext(), true);

    iter.next();

    EXPECT_EQ(*iter, 1);
    EXPECT_EQ(iter.hasNext(), false);
}

TEST(CustomArrayTest, MutateByIteratorTest) {
    Array<int> a{};

    a.insert(1);
    a.insert(2);
    a.insert(3);

    auto iter = a.iterator();

    int i = 4;

    while (iter.hasNext()) {
        *iter = i++;
        iter.next();
    }

    // Last item
    *iter = i++;

    EXPECT_EQ(a[0], 4);
    EXPECT_EQ(a[1], 5);
    EXPECT_EQ(a[2], 6);
}

TEST(CustomArrayTest, CopyAssigment) {
    Array<int> a{};

    a.insert(1);
    a.insert(2);
    a.insert(3);

    Array<int> b = a;

    EXPECT_EQ(a.size(), b.size());
    EXPECT_EQ(a.capacity(), b.capacity());
    EXPECT_EQ(a[0], b[0]);
    EXPECT_EQ(a[1], b[1]);
    EXPECT_EQ(a[2], b[2]);
}

TEST(CustomArrayTest, MoveAssigment) {
    Array<int> a{};

    a.insert(1);
    a.insert(2);
    a.insert(3);

    Array<int> b = std::move(a);

    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[2], 3);
    EXPECT_DEATH_IF_SUPPORTED(a.insert(0), "");
}