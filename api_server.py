import pydantic
import sqlalchemy
from fastapi import FastAPI, UploadFile, Request, WebSocket, Depends
from fastapi.middleware.cors import CORSMiddleware
from datetime import datetime
import os
from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from sqlalchemy.orm import Session

SQLALCHEMY_DATABASE_URL = "sqlite:///db.db"

engine = create_engine(
    SQLALCHEMY_DATABASE_URL, connect_args={"check_same_thread": False}
)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

Base = declarative_base()

# DB
class User(Base):
    __tablename__ = "user"
    key = sqlalchemy.Column(sqlalchemy.Text, unique=True, nullable=False, primary_key=True)
    machine = sqlalchemy.Column(sqlalchemy.Text, unique=True, nullable=False)
    user_name = sqlalchemy.Column(sqlalchemy.Text, unique=True, nullable=False)
    status = sqlalchemy.Column(sqlalchemy.Text, unique=True, nullable=False)
    ip = sqlalchemy.Column(sqlalchemy.Text, unique=True, nullable=False)
    domain = sqlalchemy.Column(sqlalchemy.Text, unique=True, nullable=False)
    at = sqlalchemy.Column(sqlalchemy.DATETIME, default=datetime.now)


Base.metadata.create_all(bind=engine)


# API
class UserResponse(pydantic.BaseModel):
    key: str
    machine: str
    user_name: str
    status: str
    ip: str
    domain: str
    at: datetime
    class Config:
        orm_mode = True



class UserCreate(UserResponse):
    key: str
    machine: str
    user_name: str
    status: str
    ip: str
    domain: str
    class Config:
        orm_mode = False



# storage
UPLOAD_DIR_BASE = 'media'

# app
app = FastAPI()

# middlewares
app.add_middleware(
    CORSMiddleware,
    allow_origins=['*'],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# db connection
def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

# services
def make_internal_user_storage(user_host) -> str:
    filename: str = format(datetime.now(), "%d.%m.%Y.%H_%S.jpg")
    host_storage: str = os.path.join(UPLOAD_DIR_BASE, user_host)
    if not os.path.exists(host_storage):
        os.makedirs(name=host_storage, exist_ok=True)
    return os.path.join(host_storage, filename)


async def write_image(location: str, file: UploadFile) -> None:
    with open(location, 'wb') as file_raw:
        content_bin: bytes = await file.read()
        file_raw.write(content_bin)


def create_user(db: Session, user_dict):
    db_item = User(**user_dict)
    db.add(db_item)
    db.commit()
    db.refresh(db_item)
    return db_item


def get_user(db: Session, key):
    return db.query(User).filter(User.key == key).first()

def update_user(db: Session, key, status):
    user = db.query(User).filter(User.key == key).first()
    if user:
        user.status = status
        user.at = datetime.now()
        db.commit()
    return user


def get_users(db: Session):
    return db.query(User).all()


# API endpoints
@app.get('/', response_model=list[UserResponse])
async def get_index(db: Session = Depends(get_db)):
    return get_users(db)

@app.get('/{key}', response_model=UserResponse)
async def get_index(key: str, db: Session = Depends(get_db)):
    return get_user(db, key)


@app.post('/{key}', response_model=UserResponse|None)
async def get_index(key: str, file: UploadFile, request: Request, db: Session = Depends(get_db)):
    upload_to = make_internal_user_storage(request.client.host)
    await write_image(upload_to, file)
    user = update_user(db, key, 'online')
    return user


@app.post('/', )
async def get_index(
        file: UploadFile,
        request: Request,
        db: Session = Depends(get_db)
) -> dict[str, bool]:
    upload_to = make_internal_user_storage(request.client.host)
    await write_image(upload_to, file)
    form = await request.form()
    domain = request.headers['host']
    user_dict = {
        'user_name': form.get("user"),
        'status': form.get("status"),
        'machine': form.get("machine"),
        'key': form.get("user_key"),
        "ip": str(request.client.host),
        'domain': domain
    }
    if not get_user(db, user_dict['key']):
        create_user(db, user_dict)
    return {"state": True}



