#!/usr/bin/env python3
"""
Run tests for the project.
"""
from subprocess import run
from os import path
from shutil import rmtree
from json import load as json_load
from argparse import ArgumentParser
from vt100logging import vt100logging_init, I, EX

CMAKE_PRESETS_FILE = 'CMakePresets.json'
DEFAULT_WORKFLOW_PRESET_NAME = 'Test'


def get_script_directory():
    return path.dirname(path.realpath(__file__))


def get_full_path(file_name: str) -> str:
    return path.join(get_script_directory(), file_name)


def load_presets() -> dict:
    I(f'Loading presets from {CMAKE_PRESETS_FILE}')
    with open(get_full_path(CMAKE_PRESETS_FILE)) as file:
        presets = json_load(file)
        if (presets['version'] < 6):
            raise ValueError('Unsupported version of CMakePresets.json')
    return presets


def get_build_dir_from_preset(preset: dict, workflow_name: str) -> str:
    workflow_preset = next(
        (x for x in preset['workflowPresets'] if x['name'] == workflow_name), None)
    configuration_preset_name = next(
        (x['name'] for x in workflow_preset['steps'] if x['type'] == 'configure'), None)
    build_path = next((x['binaryDir'] for x in preset['configurePresets']
                      if x['name'] == configuration_preset_name), None)
    result_build_path = path.join(get_script_directory(), build_path.replace(
        '${sourceDir}', get_script_directory()))
    I(f'Workflow preset "{workflow_name}": build path: "{result_build_path}"')
    return result_build_path


def remove_build_dir(preset: dict, workflow_name: str) -> None:
    build_dir = get_build_dir_from_preset(preset, workflow_name)
    I(f'Removing build directory: {build_dir}')
    rmtree(build_dir, ignore_errors=True)


def run_workflow(workflow_name: str) -> None:
    script_directory = get_script_directory()
    I(f'Running workflow "{workflow_name}"')
    run(['cmake', '--workflow', '--preset', workflow_name], cwd=script_directory)


def parse_args():
    parser = ArgumentParser(description='Run tests for the project.')
    parser.add_argument('--preset', default=DEFAULT_WORKFLOW_PRESET_NAME,
                        help='Name of the workflow preset to use.')
    return parser.parse_args()


def main():
    try:
        vt100logging_init('test', is_verbose=True)
        workflow_preset_name = parse_args().preset
        presets = load_presets()
        remove_build_dir(presets, workflow_preset_name)
        run_workflow(workflow_preset_name)
    except Exception as e:
        EX(e)


if __name__ == '__main__':
    main()
