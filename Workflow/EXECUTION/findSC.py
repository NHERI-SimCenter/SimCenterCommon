import pkgutil
# In Python 3.14+, function pkgutil.find_loader() is deprecated (https://gitlab.gnome.org/GNOME/pygobject/-/merge_requests/252)
# print('0' if pkgutil.find_loader('nheri_simcenter') else '1');

# Use this code instead for forward compatibility
print('0' if pkgutil.importlib.util.find_spec()('nheri_simcenter') else '1');

# We can probably adapt a try-except block to allow both methods during the transition period
# See: (https://android.googlesource.com/platform/prebuilts/python/darwin-x86/+/refs/heads/busytown-mac1010-release/lib/python3.9/pkgutil.py)