Umbra-Engine/
│
├── Source/              # Core engine code
├── Renderer/            # D3D12 / graphics pipeline
├── Core/                # Memory, utilities, systems
├── Math/                # Vectors, matrices
├── Platform/            # Windows layer
│
├── Sandbox/             # Test app (VERY IMPORTANT)
│
├── ThirdParty/          # External libs
├── Assets/              # Test assets
│
├── Build/               # Build scripts
└── README.md



---

# 🧠 What We’re Building (Your Engine Identity)

Am are building a **small, clean, learning-focused rendering + gameplay engine core**.

### 🔹 Engine Type

A **minimal real-time engine** with:

* Win32 window + input
* Direct3D 12 renderer (basic pipeline only)
* Simple game loop
* Basic ECS-style or component system (later)
* Debug-friendly structure

---

# 🎯 Your Engine Goal (Important)

> Build something that can render, move, and structure a game — not something flashy.

Think:

* Spinning cube ✅
* Camera movement ✅
* Input system ✅
* Clean architecture ✅
* Mesh shaders ❌ (not needed)
* Ray tracing ❌ (not needed)

---

# 💻 Your Hardware Reality (Dell Latitude 5400)

Your specs are **perfectly fine** for:

* Direct3D 12 basics
* Vertex + Pixel shaders
* Constant buffers
* Simple lighting
* CPU-side systems

They are **NOT meant for**:

* Mesh shaders
* Heavy real-time GI
* Advanced GPU compute pipelines (for now)

👉 That’s actually good. You’ll learn the *fundamentals* deeply.

---

# 🧱 Engine Architecture (What We Build First)

We go step-by-step. No rushing.

## Phase 1 — Foundation (You start here)

### 1. Platform Layer (Win32)

* Window creation
* Message loop
* Input (keyboard, mouse)

👉 This is your engine’s **entry point into reality**

---

### 2. Core Systems

* Game loop
* Timer (you already studied this 👀)
* Logging system (simple)

---

### 3. Graphics Layer (D3D12 — Minimal)

* Device creation
* Swap chain
* Command queue
* Render target
* Clear screen

👉 First milestone:

> **Black screen → then colored screen**

---

## Phase 2 — First Real Rendering

* Vertex buffer
* Index buffer
* Root signature
* Pipeline State Object (PSO)
* Draw a triangle → then cube

👉 Milestone:

> “I can render geometry”

---

## Phase 3 — Camera + Movement

* View matrix
* Projection matrix
* Camera class
* WASD movement

👉 Milestone:

> “I can move inside my world”

---

## Phase 4 — Engine Structure

* Game objects
* Transform (position, rotation, scale)
* Component system (lightweight, not overkill)

---

# 🧭 What You Are REALLY Learning

This is the part most people miss.

You’re not just “making an engine”, you’re learning:

* How GPU pipelines actually work
* Memory management (D3D12 = explicit control)
* How engines structure data
* How to think like a graphics programmer

---

# ⚙️ Your Tech Stack (Locked In)

Stick to this:

* **Language:** C++
* **Graphics API:** Direct3D 12
* **Platform:** Win32
* **Math:** DirectXMath


---

# 🚀 Your Engine Name (Optional but motivating)

Since you liked *Umbra*, you could use:

* **UmbraCore**
* **Umbra Engine (dev build)**
* **Umbra Engine (your original)**

---

# 🧩 Final Truth

Your laptop is not a limitation.

> If you can run Direct3D 12 → you can build a serious engine foundation.

---

If you’re ready, next move:

👉 Tell me:
**“Start Phase 1”**

and I’ll walk you step-by-step through creating your Win32 window + explaining every line (aligned with how you like learning).
