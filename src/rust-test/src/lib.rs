#[cxx::bridge]
mod ffi {
    // C++側に公開する型と関数
    extern "Rust" {
        type RustTestClass;

        fn get(&mut self) -> i32;
        fn get_rust_test_class() -> Box<RustTestClass>;
    }
}

pub struct RustTestClass {
    count: i32,
}

impl RustTestClass {
    fn get(&mut self) -> i32 {
        self.count = self.count + 1;
        return self.count;
    }
}

fn get_rust_test_class() -> Box<RustTestClass> {
    Box::new(RustTestClass {
        count: 0
    })
}
