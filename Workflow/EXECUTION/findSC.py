import pkgutil
print('0' if pkgutil.find_loader('nheri_simcenter') else '1');
