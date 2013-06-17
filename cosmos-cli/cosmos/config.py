# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#
"""Configuration handling"""

import os
import os.path as p
import sys

from pymlconf import ConfigManager
import yaml

from cosmos.util import ExitWithError


CONFIG_KEYS = ["api_url", "api_key", "api_secret"]
DEFAULT_CONFIG = {
    "api_url": "http://localhost:9000/cosmos"
}
CONFIG_DESCRIPTIONS = {
    "api_url": "Base API URL",
    "api_key": "API key",
    "api_secret": "API secret"
}


def default_config_path():
    """Get a correct configuration path, regardless if we are on a POSIX system
    or a Windows system.

    >>> "cosmosrc" in default_config_path()
    True
    """
    if os.name == "nt" and os.getenv("USERPROFILE"):
        return p.join(os.getenv("USERPROFILE").decode("mbcs"),
                      "Application Data", "cosmosrc.yaml")
    else:
        return p.expanduser("~/.cosmosrc")


def load_config(args):
    """Tries to load the configuration file or throws ExitWithError."""
    filename = args.config_file if args.config_file else default_config_path()
    try:
        config = ConfigManager(files=[filename])
    except Exception as ex:
        print "Error reading configuration from %s: %s" % (
            filename, ex.message)
        config = ConfigManager(DEFAULT_CONFIG)
    if not config.keys():
        raise ExitWithError(
            -1, ("Cosmos command is unconfigured. Use '%s configure' to " +
                 "create a valid configuration or use --config-file with a " +
                 "valid configuration file") % sys.argv[0])
    config.credentials = (config.api_key, config.api_secret)
    return config


def with_config(command):
    """Decorates a command to append the configuration as an extra argument.
    This is intended to be use as a decorator.
    """

    def decorated_command(*args, **kwargs):
        extended_args = list(args)
        extended_args.append(load_config(args[0]))
        return command(*extended_args, **kwargs)

    return decorated_command


def ask_for_setting(config, setting):
    """Interactively ask for a setting using current value as a default."""
    default = config.get(setting, "")
    answer = raw_input(
        "%s [%s]: " % (CONFIG_DESCRIPTIONS[setting], default)).strip()
    if not answer:
        answer = default
    config[setting] = answer


def ask_binary_question(question, default_answer=False):
    """Interactively asks a Y/N question on the console."""
    answer_text = "Y" if default_answer else "N"
    answer = raw_input("%s [%s]: " % (question, answer_text))
    return answer.lower() in ("y", "yes")


def command(args):
    """Create a configuration file by asking for the settings"""
    config = ConfigManager(DEFAULT_CONFIG)
    for setting in CONFIG_KEYS:
        ask_for_setting(config, setting)
    filename = default_config_path()
    if p.exists(filename) and not ask_binary_question(
        "%s already exists. Overwrite?" % filename):
        return 0
    with open(filename, 'w') as outfile:
        outfile.write(yaml.dump(dict(config.items()),
                                default_flow_style=False))
    print "Settings saved in %s" % filename
    return 0
