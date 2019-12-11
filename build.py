#!/usr/bin/env python

from cpt.packager import ConanMultiPackager
import copy

if __name__ == "__main__":
    builder = ConanMultiPackager()
    builder.add_common_builds()
    buildsWithOptions = []
    for settings, options, env_vars, build_requires, reference in builder.items:
        options['SimCenterCommonQt:MDOFwithQt3D'] = False
        buildsWithOptions.append([settings, options, env_vars, build_requires, reference])
        options = copy.deepcopy(options)
        options['SimCenterCommonQt:MDOFwithQt3D'] = True
        buildsWithOptions.append([settings, options, env_vars, build_requires, reference])
    builder.builds = buildsWithOptions
    builder.run()
