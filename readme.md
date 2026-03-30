# Computer Graphics HW2

## 控制指南 (Key Bindings)

### 模型切換與重置
| 快速鍵 | 功能說明 |
| --- | --- |
| **`F1`** | 切換並重置為 **四面體 (Tetrahedron)** 模式 |
| **`F2`** | 切換並重置為 **正方體 (Cube)** 模式 |
| **`F3`** | 切換並重置為 **外部匯入模型 (Utah Teapot)** 模式 (`teapot.obj`) |
| **`F4`** | 切換為 **層次結構動畫 (太陽系模型)** 模式，展示 Push/Pop 概念 |
| **`F5`** | **儲存場景狀態 (Save Scene)**：將目前的形狀、座標系模式、相機角度及物件的變形矩陣儲存到 `scene.txt` |
| **`F6`** | **載入場景狀態 (Load Scene)**：從 `scene.txt` 還原回先前儲存的畫面狀態 |
| **`-`** (減號)| **重置變更**，將目前的全部變形矩陣 (Transform Matrix) 歸零，回到初始狀態 |
| **`ESC`** | 離開程式 |

### 座標系統與視角
| 快速鍵 | 功能說明 |
| --- | --- |
| **`Z`** | **[重要] 切換座標系模式**。可在 **「世界座標 (Global Mode-左乘)」** 與 **「物件座標 (Local Mode-右乘)」** 間互相切換，用來觀察不同矩陣結合順序的效果。 |
| **`9` / `0`** | 旋轉整個**攝影機視角 (Camera/View)** 繞著原點公轉。 |

### 模型變換操作 (Transformation)
> 💡：所有的變形都會依賴 **`Z` 鍵** 所定義的當前座標系模式（世界座標 World space 或 區域座標 Local space）產生不同表現。

#### 平移 (Translation)
* **`Q` / `A`** : 沿著 X 軸正向 / 負向平移
* **`W` / `S`** : 沿著 Y 軸正向 / 負向平移
* **`E` / `D`** : 沿著 Z 軸正向 / 負向平移

#### 縮放 (Scaling)
* **`U` / `J`** : 沿著 X 軸放大 / 縮小
* **`I` / `K`** : 沿著 Y 軸放大 / 縮小
* **`O` / `L`** : 沿著 Z 軸放大 / 縮小

#### 旋轉 (Rotation)
* **`R` / `F`** : 繞著 X 軸正向 / 負向旋轉
* **`T` / `G`** : 繞著 Y 軸正向 / 負向旋轉
* **`Y` / `H`** : 繞著 Z 軸正向 / 負向旋轉

---

## 開發細節
補全了以下電腦圖學核心矩陣演算法的實作：
1. **Model Transform** (`swTranslate`, `swRotateX/Y/Z`, `swScale`, `swRotate`)
2. **View Transform** (`swLookAt`)
3. **Projection Transform** (`swPerspective`)

### 開關設定
在 `main.cpp` 開頭中可以透過修改 `STEP2` 及 `STEP3` 常數，開關 `true` 或 `false` 來啟用或關閉「視角轉換 (View)」與「透視投影 (Projection)」。

### OBJ 動態讀取
本專案內建輕量化 OBJ parser，並支援 Quad 或 Triangle 面解析。若要更改載入模型，只需替換工作目錄中的 `.obj` 檔案，或自定義 `LoadOBJ()` 的路徑即可。
