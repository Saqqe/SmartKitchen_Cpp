import subprocess, time

print"Hej"
p = subprocess.Popen("sudo python test.py", shell = True)
time.sleep(5)
p.kill()
