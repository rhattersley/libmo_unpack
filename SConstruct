VERSION = '3.0.0'

AddOption('--prefix', dest='prefix', nargs=1, type='string', action='store',
          metavar='DIR', help='installation prefix')
env = Environment(PREFIX=GetOption('prefix'))
conf = Configure(env)
if not conf.CheckLib('m'):
    print "ERROR: Unable to find 'm' library."
    Exit(1)
print env['PREFIX']
lib = SharedLibrary('mo_unpack', Glob('src/*.c'), SHLIBVERSION=VERSION)
install_shared = env.InstallVersionedLib('$PREFIX/lib', lib, SHLIBVERSION=VERSION)
env.Alias('install', install_shared)
