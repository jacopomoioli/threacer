![logo-black](docs/images/logo_black.png)

# Threacer - The Threat Tracer
Threacer is an educational proof of concept of a simple anti malware for Windows that emphasizes behavior-based detection.

## Why
> What I cannot create, I do not understand<br>
> _Richard Feynman_

As an offensive security professional wannabe, I want to dive deeper into the runtime detection techniques (aka "Robust detection") employed by modern EDRs.

Although **this project does not aim to become a production ready tool**, it is certainly useful as an educational project to understand the fundamentals behind the security system I try to evade in my day job. In addition, most publicly available antivirus projects focus on brittle detection (signature-based, artifact-focused) techniques.

Also, as a side effect, this project allows me to 
- explore Windows internals
- improve system programming skills
- introduce to kernel programming

## Architecture
The core architecture of Threacer is based on what is described during the Chapter 1 of "[Evading EDR: The Definitive Guide to Defeating Endpoint Detection Systems](https://nostarch.com/evading-edr)" by [Matt Hand](https://www.linkedin.com/in/handm/): it's composed of
- a userland agent service (TODO)
- an userland hooking DLL (PoC done)
- a kernel mode driver (TODO)
- a static scanner (TODO)

The goal is to improve the architecture, add more telemetry sources and a more efficient detection logic incrementally, step by step.

### Hooking DLL
Uses MS Detours to hook some Windows APIs:
- ntdll.dll
    - NtCreateUserProcess

The currently implemented detour consists in a simple function that prints the called API on console and debugger (via `OutputDebugStringA`)

## Demo
The `demos` folder contains some simple programs for testing (or evading) the functionality of Threacer.

### `NtCreateUserProcess_Hooking`
A simple program that creates a new notepad process using the `CreateProcessA` WINAPI (that calls the hooked `NtCreateUserProcess` under the hood). 

Used as a target to test the NTDLL hooking capability of Threacer.

### `Ntdll_Unhooking`
Similar to `NtCreateUserProcess_Hooking`, but evades Threacer by unhooking NTDLL.dll.

First, it calls `CreateProcessA` normally. The call will be hooked by Threacer.

Then, executes the API Unhooking procedure:
1. Maps from disk a "clean" copy of NTDLL in memory
2. Gets the base address of the currently loaded ntdll.dll (with threacer's hooks)
3. Parses PE headers of the currently loaded ntdll in order to find the .text section
4. Overwrites the old (and hooked) .text section with the one of the clean copy

After these steps, `CreateProcessA` is called again, but because of the unhooking, Threacer detour won't be executed


## Development History
- 2025-10-04: API Unhooking demo
- 2025-10-04: Hooking of ntdll.dll PoC
- 2025-09-15: Hooking DLL PoC (taken from previous project "detraces")