# python版 ブロック認識への取り組み

## 開発環境構築

python用のOpenCVをインストールする。

```
brew install pyenv
echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.bash_profile
echo 'export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.bash_profile
echo 'eval "$(pyenv init -)"' >> ~/.bash_profile
source ~/.bash_profile
pyenv install anaconda3-4.2.0
pyenv global anaconda3-4.2.0
sudo conda install -c menpo opencv3
```
