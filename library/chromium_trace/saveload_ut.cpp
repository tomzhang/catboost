#include "saveload.h"

#include <library/unittest/registar.h>

#include <util/memory/pool.h>
#include <util/stream/str.h>

using namespace NChromiumTrace;

namespace {
    template<typename T>
    static TString SaveToString(const T& value) {
        TString data;
        TStringOutput out(data);
        ::Save(&out, value);
        return data;
    }

    template<typename T>
    static T LoadFromString(const TString& data, TMemoryPool& pool) {
        TStringInput in(data);
        T result;
        ::Load(&in, result, pool);
        return result;
    }

    template<typename T>
    static void TestSaveLoad(const T& value) {
        TMemoryPool pool(4096);
        TString data = SaveToString(value);
        T result = LoadFromString<T>(data, pool);
        UNIT_ASSERT_EQUAL(value, result);
    }
}

SIMPLE_UNIT_TEST_SUITE(SaveLoad) {

SIMPLE_UNIT_TEST(EventArgs_Arg) {
    using TArg = TEventArgs::TArg;

    TestSaveLoad(TArg(STRINGBUF("TestI64Arg"), i64(0xdeadbeef)));
    TestSaveLoad(TArg(STRINGBUF("TestDoubleArg"), double(3.1415)));
    TestSaveLoad(TArg(STRINGBUF("TestStringArg"), STRINGBUF("Hello World!")));
}

SIMPLE_UNIT_TEST(EventArgs) {
    TestSaveLoad(TEventArgs());
    TestSaveLoad(TEventArgs()
        .Add(STRINGBUF("TestI64Arg"), i64(0xdeadbeef))
    );
    TestSaveLoad(TEventArgs()
        .Add(STRINGBUF("TestI64Arg"), i64(0xdeadbeef))
        .Add(STRINGBUF("TestDoubleArg"), double(3.1415))
        .Add(STRINGBUF("TestI64Arg"), STRINGBUF("Hello World!"))
    );
}

SIMPLE_UNIT_TEST(DurationBeginEvent) {
    TestSaveLoad(TDurationBeginEvent {
        TEventOrigin::Here(),
        "TestEvent",
        "Test,Sample",
        TEventTime::Now(),
        TEventFlow {
            EFlowType::Producer,
            0xdeadbeef,
        },
    });
}

SIMPLE_UNIT_TEST(DurationEndEvent) {
    TestSaveLoad(TDurationEndEvent {
        TEventOrigin::Here(),
        TEventTime::Now(),
        TEventFlow {
            EFlowType::Producer,
            0xdeadbeef,
        }
    });
}

SIMPLE_UNIT_TEST(DurationCompleteEvent) {
    TestSaveLoad(TDurationCompleteEvent {
        TEventOrigin::Here(),
        "TestEvent",
        "Test,Sample",
        TEventTime::Now(),
        TEventTime::Now(),
        TEventFlow {
            EFlowType::Producer,
            0xdeadbeef,
        }
    });
}

SIMPLE_UNIT_TEST(InstantEvent) {
    TestSaveLoad(TInstantEvent {
        TEventOrigin::Here(),
        "TestEvent",
        "Test,Sample",
        TEventTime::Now(),
        EScope::Process,
    });
}

SIMPLE_UNIT_TEST(AsyncEvent) {
    TestSaveLoad(TAsyncEvent {
        EAsyncEvent::Begin,
        TEventOrigin::Here(),
        "TestEvent",
        "Test,Sample",
        TEventTime::Now(),
        0xdeadbeef,
    });
}

SIMPLE_UNIT_TEST(CounterEvent) {
    TestSaveLoad(TCounterEvent {
        TEventOrigin::Here(),
        "TestEvent",
        "Test,Sample",
        TEventTime::Now(),
    });
}

SIMPLE_UNIT_TEST(MetadataEvent) {
    TestSaveLoad(TMetadataEvent {
        TEventOrigin::Here(),
        "TestEvent",
    });
}

SIMPLE_UNIT_TEST(EventWithArgs) {
    TestSaveLoad(TEventWithArgs {
        TCounterEvent {
            TEventOrigin::Here(),
            "TestEvent",
            "Test,Sample",
            TEventTime::Now(),
        },
    });
    TestSaveLoad(TEventWithArgs {
        TCounterEvent {
            TEventOrigin::Here(),
            "TestEvent",
            "Test,Sample",
            TEventTime::Now(),
        },
        TEventArgs()
            .Add("Int64Arg", i64(0xdeadbeef))
    });
    TestSaveLoad(TEventWithArgs {
        TMetadataEvent {
            TEventOrigin::Here(),
            "TestEvent",
        },
    });
    TestSaveLoad(TEventWithArgs {
        TMetadataEvent {
            TEventOrigin::Here(),
            "TestEvent",
        },
        TEventArgs()
            .Add("Int64Arg", i64(0xdeadbeef))
    });
}

} // SIMPLE_UNIT_TEST_SUITE(SaveLoad)
