import json
import requests
from block import block

class BlockInfoEncoder(json.JSONEncoder):
  def default(self, o):
    if isinstance(o, block.info):
      return { 'color' : o.color, 'width' : o.width }
    return super(BlockInfoEncoder, self).default(o)

def post(blocks):
  d = json.dumps({ "orders" : blocks }, cls = BlockInfoEncoder)
  print(d)
  try:
    res = requests.post('http://127.0.0.1:5001/api/show', d)
    print(res.status_code)
  except requests.exceptions.ConnectionError as e:
    print e