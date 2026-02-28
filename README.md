# SecurePay Payment Processing

[![Build and test](https://github.com/subhashpolisetti/SecurePay-Finance-Tracker-with-Payment--Gateway/actions/workflows/ci.yml/badge.svg)](https://github.com/subhashpolisetti/SecurePay-Finance-Tracker-with-Payment--Gateway/actions/workflows/ci.yml)

A C++17 / Qt 6 desktop payment gateway prototype with card validation, rule-based fraud detection,
refunds, reporting and SQLite persistence. Built for CMPE 202 (Software Systems Engineering) to
practice object-oriented design patterns on a realistic domain.

The bank and encryption steps are simulated; no real payment network is contacted.

## Features

- **Payments** by credit card, debit card or digital wallet, sent to a chosen merchant
- **Card validation** before authorization: Luhn checksum, 13–19 digit length, expiry date, CVV format
- **Fraud scoring** from four rules (large amount, suspicious billing address, high-risk method,
  repeated payments within 60 seconds); medium and high risk payments raise fraud alerts, and
  high risk payments are held for review
- **Refunds**, full or partial, tracked per transaction through the transaction state machine
- **Reports** (transaction history, refunds, fraud alerts, customer spending, merchant earnings,
  daily and monthly summaries) exported to CSV or JSON
- **SQLite persistence layer** using prepared statements; only display-safe payment details
  (last four digits, cardholder, expiry) are stored, never the full card number or CVV

## Architecture

```
SecurePay-Payment-Processing/
├── src/core/   securepay_core: domain logic, plain C++17 + SQLite (no Qt)
├── src/gui/    securepay_gui: Qt Widgets main window and dialogs
├── src/main.cpp
└── tests/      Qt Test suites run through CTest
```

Payment flow: `MainWindow` → `AppController` → `PaymentGateway` → `FraudSystem` (risk level)
→ `Bank` (authorization) → transaction state set to Approved, Declined or Flagged for Review,
with a `FraudAlert` recorded for medium and high risk.

| Pattern | Where |
|---|---|
| Strategy | `PaymentMethod` (card / wallet), report generators, CSV / JSON exporters |
| State | `TransactionState` (Pending, Approved, Declined, Flagged, Refunded, Partially Refunded) |
| Factory | `TransactionFactory`, `PaymentMethodFactory`, `RefundFactory`, `FraudAlertFactory` |
| Observer | `TransactionObserver` notified by `PaymentGateway` |
| Command | `FullRefundCommand`, `PartialRefundCommand` run by `RefundManager` |
| Singleton | `Bank`, `FraudSystem`, `RefundManager`, `ReportManager` |
| Facade | `PaymentGatewayFacade` |
| Decorator | `EncryptedTransaction`, `LoggedTransaction` |
| Lazy evaluation | `LazyReport` generates report data on first use |

## Build

Requirements: CMake 3.16+, a C++17 compiler, Qt 6 (Core, Gui, Widgets, Charts, Test), SQLite 3.

```bash
cd SecurePay-Payment-Processing
cmake -S . -B build
cmake --build build
./build/SecurePay
```

On macOS with Homebrew Qt, pass `-DCMAKE_PREFIX_PATH=$(brew --prefix qt)` and build with Apple
clang (`-DCMAKE_CXX_COMPILER=clang++`); Homebrew GCC cannot link Qt's macOS frameworks.
If linking fails with `framework 'AGL' not found` (Qt 6.8 on newer macOS SDKs), also pass
`-DWrapOpenGL_AGL=$(xcrun --show-sdk-path)/System/Library/Frameworks/OpenGL.framework`.

On Ubuntu 24.04:

```bash
sudo apt-get install cmake g++ qt6-base-dev qt6-charts-dev libgl-dev libsqlite3-dev
```

## Tests

```bash
ctest --test-dir build --output-on-failure
```

Nine Qt Test suites (64 test cases) cover payment method validation, bank authorization, fraud
rules, the payment gateway, the transaction state machine, refunds, report generation and export,
SQLite round trips including SQL-injection and stored-data checks, and the main window driven
headless (`QT_QPA_PLATFORM=offscreen`). GitHub Actions runs the build and tests on every push.

Test card numbers that pass validation: `4111111111111111` (Visa), `5500000000000004`
(Mastercard), `378282246310005` (Amex), with any future expiry such as `12/30` and a 3–4 digit CVV.

## Limitations

- Bank authorization and transaction encryption are simulated.
- The desktop app keeps data in memory for the session; `SQLiteDataManager` is implemented and
  tested but not yet loaded and saved by the app at startup and exit.
- Amounts are stored as `double`; refund comparisons allow half a cent of rounding.
