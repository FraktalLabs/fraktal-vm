# Fraktal VM

Runs a Fraktal VM client to host compute

Stand-alone C++20 executable and library used to provide Fraktal VM compute.
Fraktal VM is a superset of the EVM, which allows parallelism and concurrency
in contract execution. The design is inspired by golang's goroutines & runtime.

## Table of Contents

- [Install](#install)
- [Usage](#usage)
- [Dependencies](#dependencies)
- [Testing](#testing)
- [Details](#details)
- [Media](#media)
- [Maintainer](#maintainer)
- [License](#license)

## Install

```
make all
```

**NOTE:** [Dependencies](#dependencies) must be compiled & linked into compilation aswell.

## Usage

fraktal-vm comes with one main command to run :

```
./bin/fraktal-vm run --stateSnapshot ./path/to/snapshot.json --txPoolSnapshot ./path/to/transactions.csv
```

## Dependencies

- [intx][intx] : 256-bit (32-byte) unsigned integers
- [ethash][ethash] : Ethereum hash functions
- [evm-cpp-utils][evm-cpp-utils] : EVM state & account types + utils
- [proc-evm][proc-evm] : EVM execution & Call context
- [fraktal-state-db][fraktal-state-db] : Fraktal VM nonce-locked state & accounts

## Testing

This repo contains various tests under `./test/` to make sure things are working.
To run use :

```
make run-fraktal-vm-test
# After txs run, Ctrl-C to stop VM runtime
```

Check the diff in `./test/snapshot.json` to see if things processed properly.

## Details

Running the Fraktal-VM does the following :

1. Load the `FraktalState` from a snapshot
2. Load the `TxPool` from a snapshot
3. Create a pool of `EVMThread` objects, making the `threadPool`
4. Start the `TxPoolServer` in its own thread to accept txs into `TxPool`
5. Start spinning each thread in the `threadPool`
6. Wait till Fraktal VM runtime is killed, then snapshot `FraktalState` + `TxPool`

Each `EVMThread` does the following :
1. Get transaction from pool, if none available then spin
2. Run the transaction using `CallContext` on a `FraktalAccount`.
3. Remove Tx from pool
4. Repeat

**NOTE**: Not currently implemented, but Tx processing will be changed to coroutines processing.

## Media

![Fraktal VM Runtime](https://github.com/FraktalLabs/docs/blob/master/images/fraktal-vm/fraktal-vm-runtime.jpg)

## Maintainer

Brandon Roberts [@b-j-roberts]

## License

[MIT][MIT]


[intx]: https://github.com/chfast/intx
[ethash]: http://github.com/chfast/ethash
[evm-cpp-utils]: https://github.com/FraktalLabs/evm-cpp-utils
[proc-evm]: https://github.com/FraktalLabs/proc-evm
[fraktal-state-db]: https://github.com/FraktalLabs/fraktal-state-db
[MIT]: LICENSE
[@b-j-roberts]: https://github.com/b-j-roberts
