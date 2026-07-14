"""Ensure build directory exists before build (fixes WSL .sconsign error with embed_txtfiles)."""
Import("env")
import os
os.makedirs(env["PROJECT_BUILD_DIR"], exist_ok=True)
