#[cxx::bridge(namespace = "rust::images")]
mod ffi {
    // C++と共有する個々の画像データ情報
    #[derive(Clone)]
    struct ImageEntry {
        full_path: String,
        file_name: String,
        stem: String,
    }

    extern "Rust" {
        type ImageCatalog;

        fn new_catalog() -> Box<ImageCatalog>;
        // 指定したパス内のPNGをスキャンして内部リストを更新
        fn scan_directory(&mut self, path: &str);
        // 見つかったファイル情報のリストをC++に返す
        fn get_entries(&self) -> Vec<ImageEntry>;
    }
}

pub struct ImageCatalog {
    entries: Vec<ffi::ImageEntry>,
}

impl ImageCatalog {
    fn new_catalog() -> Box<Self> {
        Box::new(Self { entries: Vec::new() })
    }

    fn scan_directory(&mut self, path: &str) {
        self.entries.clear();
        let read_dir = match std::fs::read_dir(path) {
            Ok(d) => d,
            Err(_) => return,
        };

        for entry in read_dir.flatten() {
            let p = entry.path();
            // pngファイルのみ対象
            if p.extension().map_or(false, |ext| ext == "png") {
                let full_path = p.to_string_lossy().into_owned();
                let file_name = p.file_name().unwrap().to_string_lossy().into_owned();
                let stem = p.file_stem().unwrap().to_string_lossy().into_owned();

                self.entries.push(ffi::ImageEntry {
                    full_path,
                    file_name,
                    stem,
                });
            }
        }
    }

    fn get_entries(&self) -> Vec<ffi::ImageEntry> {
        self.entries.clone()
    }
}

// 初期化用関数
fn new_catalog() -> Box<ImageCatalog> {
    ImageCatalog::new_catalog()
}
