make -C ./prototype
python -c "import sys; print(sys.version)" & > /dev/null

if [ $? -eq 0 ]; then
  python gui/gui.py
else
  python3 gui/gui.py
fi