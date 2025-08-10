#!/usr/bin/env python3
# -*- coding: utf-8 -*-

print("Starting test...")

try:
    import pandas as pd
    print("✓ pandas imported")
except ImportError as e:
    print(f"✗ pandas error: {e}")

try:
    from sqlalchemy import create_engine
    print("✓ sqlalchemy imported")
except ImportError as e:
    print(f"✗ sqlalchemy error: {e}")

try:
    import matplotlib.pyplot as plt
    print("✓ matplotlib imported")
except ImportError as e:
    print(f"✗ matplotlib error: {e}")

try:
    from config import DATABASE_CONFIG
    print("✓ config imported")
    print(f"Database config: {DATABASE_CONFIG['host']}:{DATABASE_CONFIG['port']}")
except ImportError as e:
    print(f"✗ config error: {e}")

print("Test completed.")
