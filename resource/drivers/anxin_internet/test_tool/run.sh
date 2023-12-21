#!/bin/bash
LD_LIBRARY_PATH=../lib:$LD_LIBRARY_PATH ./bin/ama_test --log-to-console --cfg-name ./etc/ama.json  --data-to-console --data-to-csv --snapshot --index-snapshot --tick-order --tick-execution --bond-snapshot --hk-snapshot --order-book --hkex-snapshot --hkex-index-snapshot --hkex-tick-execution --hkex-order-snapshot --hkex-order-broker-snapshot --usa-stock-snapshot --usa-order-snapshot --bond-tick-order --bond-tick-execution --bond-quoted-tick-order --bond-quoted-tick-execution

