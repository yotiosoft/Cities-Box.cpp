#[cxx::bridge]
mod ffi {
    // C++側に公開する型と関数
    extern "Rust" {
        type EnemyManager;

        fn new_enemy_manager() -> Box<EnemyManager>;
        fn spawn_enemy(&mut self, x: f32, y: f32);
        fn update_all(&mut self);
        fn get_enemy_count(&self) -> usize;
    }
}

pub struct EnemyManager {
    enemies: Vec<Enemy>, // Rustの構造体
}

impl EnemyManager {
    fn spawn_enemy(&mut self, x: f32, y: f32) { /* 処理 */ }
    fn update_all(&mut self) { /* 処理 */ }
    fn get_enemy_count(&self) -> usize { self.enemies.len() }
}

fn new_enemy_manager() -> Box<EnemyManager> {
    Box::new(EnemyManager { enemies: Vec::new() })
}
