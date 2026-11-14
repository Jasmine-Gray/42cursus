export type AvatarOption = {
  id: string
  label: string
  src: string
  category: 'person' | 'animal'
  description: string
}

export const avatarOptions: AvatarOption[] = [
  { id: 'child-detective', label: '路地の案内人', description: '小さな足取りで、街の秘密を追う。', src: '/avatars/child-detective.webp', category: 'person' },
  { id: 'teen-detective', label: '見習い探偵', description: 'まだ知らない真相を、その手に。', src: '/avatars/teen-detective.webp', category: 'person' },
  { id: 'yu', label: '霧街の探偵', description: '静かな観察眼で、嘘の輪郭を捉える。', src: '/avatars/yu.webp', category: 'person' },
  { id: 'minato', label: '記録官', description: '一つの言葉も、取りこぼさない。', src: '/avatars/minato.webp', category: 'person' },
  { id: 'aoi', label: '新聞記者', description: '会話の裏に、見出しを探す。', src: '/avatars/aoi.webp', category: 'person' },
  { id: 'ren', label: '夜の配達人', description: '沈黙の奥に、秘密を忍ばせる。', src: '/avatars/ren.webp', category: 'person' },
  { id: 'yuna', label: '追跡者', description: '迷いのない一歩で、核心へ。', src: '/avatars/yuna.webp', category: 'person' },
  { id: 'kei', label: '時計技師', description: 'わずかな違いから、仕組みを読む。', src: '/avatars/kei.webp', category: 'person' },
  { id: 'midlife-detective', label: '社交界の策士', description: '笑みの向こうに、鋭い推理。', src: '/avatars/midlife-detective.webp', category: 'person' },
  { id: 'senior-detective', label: '老練の探偵', description: '長い夜も、真相までの道のり。', src: '/avatars/senior-detective.webp', category: 'person' },
  { id: 'cat', label: '路地の黒猫', description: '闇の中で、金色の瞳が光る。', src: '/avatars/cat.webp', category: 'animal' },
  { id: 'shiba', label: '忠実な番犬', description: '見逃さない。逃がさない。', src: '/avatars/shiba.webp', category: 'animal' },
  { id: 'rabbit', label: '月夜のうさぎ', description: '小さな物音にも、耳を澄ます。', src: '/avatars/rabbit.webp', category: 'animal' },
  { id: 'owl', label: '夜の番人', description: '翼を広げ、夜を見渡す。', src: '/avatars/owl.webp', category: 'animal' },
]

export const avatarByPlayerId: Record<string, string> = Object.fromEntries(
  avatarOptions.map((avatar) => [avatar.id === 'yu' ? 'self' : avatar.id, avatar.src]),
)
