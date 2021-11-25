#!/usr/bin/env python3
import string
import os
from collections import namedtuple

generate_for_languages = os.environ.get(
    'THEMIS_TEST_LANGUAGES', 'ruby,python,js,node,go,php,rust')
generate_for_languages = generate_for_languages.split(',')

LanguageSetting = namedtuple(
    'LanguageSetting', ['name', 'command', 'script_path', 'extension'])

languages = [
    LanguageSetting(name='ruby', command='ruby', script_path='./tools/ruby', extension='rb'),
    LanguageSetting(name='python', command='python3', script_path='./tools/python', extension='py'),
    LanguageSetting(name='js', command='node', script_path='./tools/js/wasm-themis', extension='js'),
    LanguageSetting(name='node', command='node', script_path='./tools/js/jsthemis', extension='js'),
    LanguageSetting(name='go', command='env', script_path='./tools/go', extension='go.compiled'),
    LanguageSetting(name='php', command='php -f', script_path='./tools/php', extension='php'),
    LanguageSetting(name='rust', command='env', script_path='./tools/rust', extension='rust'),
]

templates = {
    'smessage_signing': './tests/_integration/integration_smessage_signing.template',
    'smessage_encryption': './tests/_integration/integration_smessage_encryption.template',
    'scell_token_protect': './tests/_integration/integration_scell_token_protect.template',
    'scell_seal': './tests/_integration/integration_scell_seal.template',
    'scell_seal_pw': './tests/_integration/integration_scell_seal_pw.template',
    'scell_context_imprint': './tests/_integration/integration_scell_context_imprint.template',
}

def render_snippet(template, setting1, setting2):
    return template.safe_substitute(
        language_name1=setting1.name, language_name2=setting2.name,
        command1=setting1.command, command2=setting2.command,
        script_path1=setting1.script_path, script_path2=setting2.script_path,
        script1_extension=setting1.extension, script2_extension=setting2.extension)

base_template_path = './tests/_integration/base.template'
with open(base_template_path) as f:
    base_template = string.Template(f.read())

with open('./tests/_integration/integration_total.template') as f:
    script_runner_template = string.Template(f.read())

scripts_runners = []
# generate scripts for each operation
for snippet_name in templates:
    snippets = []
    with open(templates[snippet_name]) as f:
        snippet_template = string.Template(f.read())
    for setting1 in languages:
        for setting2 in languages:
            if (setting1.name not in generate_for_languages or
                    setting2.name not in generate_for_languages):
                continue
            snippet = render_snippet(snippet_template, setting1, setting2)
            snippets.append(snippet)

    script = base_template.substitute(commands='\n'.join(snippets))

    script_path = './tests/_integration/integration_{}.sh'.format(snippet_name)
    with open(script_path, 'w') as f:
        f.write(script)
    scripts_runners.append(script_runner_template.safe_substitute(
        script_path=script_path, test_name=snippet_name))

with open('./tests/_integration/integration_total.sh', 'w') as f:
    f.write(base_template.safe_substitute(commands='\n'.join(scripts_runners)))

print('done')
