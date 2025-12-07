# 🐚 Pukhraj Shell — A Custom Shell built from Scratch (C++23)

A from-scratch implementation of a Unix-style interactive shell written in **modern C++23**, featuring job control, pipelines, command parsing, auto-complete, history, and more.

---

## 🚀 Features

### ✅ Core Shell Functionality
- Execute system commands (`ls`, `echo`, `cat`, etc.)
- Parse arguments (e.g., `echo hello world`)
- Search PATH to locate executables
- Run programs using fork/exec system calls

### ⚙️ Job Control
- Foreground job execution
- Background jobs using `command &`
- Process tracking + cleanup

### 🧵 Pipelines
Supports multi-process pipelines:
```bash
ls | grep cpp | wc -l
```


### 📁 Directory Navigation (Built-ins)
- `cd <path>`
- `pwd`
- `exit`

### 🧠 Built-in Commands
- `cd`
- `pwd`
- `exit`
- `help`

### ✨ Quality-of-Life Features
- **Command history** (Up/Down arrows)
- **Auto-complete** (Tab)
- **Line editing** via GNU `readline`
- Optional colored prompt

### 📚 Error Handling
- Unknown commands  
- Permission denied  
- Broken pipelines  
- Failed directory changes  
- Missing executables  

---

## 🛠 Tech Stack

| Component | Purpose |
|----------|---------|
| **C++23** | Core shell implementation |
| **CMake** | Build configuration |
| **vcpkg (manifest mode)** | Dependency management |
| **readline** | History, autocomplete, input editing |
| **GitHub Codespaces** | Development environment |

---

## 📦 Project Structure

```
Pukhraj-shell/
├── src/                      # C++ source files
├── include/                  # Header files
├── your_program.sh           # Build + run script
├── CMakeLists.txt            # Build configuration
├── vcpkg.json                # List of dependencies
├── vcpkg-configuration.json  # vcpkg config
├── build/                    # Auto-generated build folder
└── README.md                 # This file
```

---


## 🏗 System Architecture  
This shell follows a **modular multi-layer architecture** similar to real Unix shells.

### **1. Input Layer (Readline + Prompt)**
- The `readline()` library handles:
  - Interactive prompt
  - History navigation
  - Autocomplete hooks  
- Produces a raw string command.

### **2. Parsing Layer**
- The parser splits input into:
  - Tokens  
  - Commands  
  - Redirections  
  - Pipelines  
- Example:
```bash
echo hello > out.txt | wc -c
```
is parsed into a **vector of Command objects**.

### **3. Execution Layer**
- Handles:
- Running built-in commands  
- Spawning child processes  
- Pipe creation (`pipe()` + `dup2()`)  
- Input/output redirection (`>`, `>>`, `<`)
- Each pipeline stage runs in its own forked process.

### **4. Built-ins Layer**
- Commands executed without forking:
- `cd`
- `exit`
- `history`
- Faster and avoids unnecessary child processes.

### **5. System Integration Layer**
- Works directly with:
- POSIX `execvp`
- File descriptors
- Environment variables
- Signal handling (SIGINT)

### **Architecture Diagram**

```bash
    ┌─────────────────────────┐
    │   Input & Prompt Layer   │
    └─────────────┬───────────┘
                  │
                  ▼
    ┌─────────────────────────┐
    │     Parser / Tokenizer   │
    └─────────────┬───────────┘
                  │
                  ▼
    ┌─────────────────────────┐
    │     Command Executor     │
    └─────┬──────────┬─────────┘
          │          │
          ▼          ▼
 ┌────────────┐ ┌─────────────┐
 │ Built-ins  │ │ External CMD │
 │ (cd, pwd,  │ │ via fork+exec│
 │ exit, etc) │ └─────────────┘
 └────────────┘       │
                      ▼
              ┌─────────────────┐
              │    OS Kernel    │
              └─────────────────┘


```
---

# 🛠 How to Build (Runs Online in Codespaces!)

1. Open the repository in **GitHub Codespaces**
2. When the container finishes setting up, open a terminal
3. Run:

```bash
chmod +x ./your_program.sh

./your_program.sh
```

## Authors

Made with ❤️ by [pukhraj102](https://github.com/pukhraj102)
