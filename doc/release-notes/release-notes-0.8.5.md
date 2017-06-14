Bitcoin-Qt versbitcoin 0.8.5 is now available from:
  http://sourceforge.net/projects/bitcoin/files/Bitcoin/bitcoin-0.8.5/

This is a maintenance release to fix a critical bug;
we urge all users to upgrade.

Please report bugs using the issue tracker at github:
  https://github.com/bitcoin/bitcoin/issues


How to Upgrade
--------------

If you are running an older versbitcoin, shut it down. Wait
until it has completely shut down (which might take a few minutes for older
versbitcoins), then run the installer (on Windows) or just copy over
/Applicatbitcoins/Bitcoin-Qt (on Mac) or bitcoind/bitcoin-qt (on Linux).

If you are upgrading from versbitcoin 0.7.2 or earlier, the first time you
run 0.8.5 your blockchain files will be re-indexed, which will take
anywhere from 30 minutes to several hours, depending on the speed of
your machine.

0.8.5 Release notes
===================

Bugs fixed
----------

Transactbitcoins with versbitcoin numbers larger than 0x7fffffff were
incorrectly being relayed and included in blocks.

Blocks containing transactbitcoins with versbitcoin numbers larger
than 0x7fffffff caused the code that checks for LevelDB database
inconsistencies at startup to erroneously report database
corruptbitcoin and suggest that you reindex your database.

This release also contains a non-critical fix to the code that
enforces BIP 34 (block height in the coinbase transactbitcoin).

--

Thanks to Gregory Maxwell and Pieter Wuille for quickly
identifying and fixing the transactbitcoin versbitcoin number bug.