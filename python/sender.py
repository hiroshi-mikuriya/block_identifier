import json
import requests
from block import block

class BlockInfoEncoder(json.JSONEncoder):
  def default(self, o):
    if isinstance(o, block.info):
      return { 'color' : o.color, 'width' : o.width }
    return super(BlockInfoEncoder, self).default(o)

def post(blocks):
  try:
    url = 'http://127.0.0.1:5001/api/show'
    d = json.dumps({ "orders" : blocks }, cls = BlockInfoEncoder)
    h = { 'content-type': 'application/json' }
    print(d)
    res = requests.post(url, d, headers = h)
    print(res.status_code)
  except requests.exceptions.ConnectionError as e:
    print e